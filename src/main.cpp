#include <Arduino.h>

#include "devices/DeviceManager.h"
#include "devices/SmartLight.h"
#include "devices/SmartSwitch.h"

#include "sensors/EnvironmentManager.h"
#include "core/RuleEngine.h"
#include "core/AlarmManager.h"

#include "utils/WiFiManager.h"
#include "utils/GPIOController.h"

#include "web/WebServer.h"
#include "utils/DataLogger.h"
#include "utils/SPIFFSStorage.h"
#include "core/DataProcessor.h"
#include "utils/ConfigManager.h"
#include "config.h"

#include "sensors/TemperatureSensor.h"
#include "sensors/HumiditySensor.h"
#include "sensors/LightSensor.h"
#include "sensors/SmokeSensor.h"
#include <DHT.h>

// ===== 全局对象 =====
SPIFFSStorage spiffsStorage;
ConfigManager configManager(spiffsStorage);
WiFiManager wiFiManager(configManager.getWiFiSsid(), configManager.getWiFiPassword());
DeviceManager deviceManager;                 // ✔ 正确：无参构造
EnvironmentManager environmentManager;
RuleEngine ruleEngine;
DataLogger dataLogger(&spiffsStorage, "/logs.txt", configManager.getMaxLogSize());
AlarmManager alarmManager(&dataLogger, &spiffsStorage);
WebServer webServer(deviceManager, environmentManager, wiFiManager, alarmManager, spiffsStorage, configManager);


// ===== 引脚定义 =====
#define RELAY_PIN   5
#define DHT_PIN     14
#define LIGHT_PIN   32
#define SMOKE_PIN   33
#define BUZZER_PIN  17
const int SMART_LIGHT_PIN = 2;
#define SMART_SWITCH_PIN 12
DHT dht(DHT_PIN, DHT11); 


// ===== 初始化设备 =====
void initializeDevices() {
  bool ok;

  ok = deviceManager.addDevice(
      new SmartLight("light_001", SMART_LIGHT_PIN));
  Serial.println(ok ? "Add light_001: OK" : "Add light_001: FAIL");

  ok = deviceManager.addDevice(
      new SmartSwitch("switch_001", SMART_SWITCH_PIN));
  Serial.println(ok ? "Add switch_001: OK" : "Add switch_001: FAIL");

  // 蜂鸣器 (作为开关设备)
  // 用户反馈：activeLow=false 时一直响，尝试改回 true
  // 硬件连接：GND, VCC(3V3), IO
  // activeLow=true 意味着：
  //   初始化状态 (关): 输出 HIGH (3.3V)
  //   开启状态 (开): 输出 LOW (0V)
  // 如果蜂鸣器是低电平触发（Low Trigger），则应设为 true
  ok = deviceManager.addDevice(
      new SmartSwitch("buzzer_001", BUZZER_PIN, true));
  Serial.println(ok ? "Add buzzer_001: OK" : "Add buzzer_001: FAIL");
}

// ===== Arduino setup =====
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== SETUP START ===");

  initializeDevices();

  Serial.print("Device count after init: ");
  Serial.println(deviceManager.getAllDevices().size());
 
    // ====== 【加 DHT 硬件直读测试】 ======
  Serial.println("=== DHT raw test ===");
  dht.begin();                 // 必须
  delay(2000);                 //  DHT 上电稳定
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  Serial.print("Raw DHT Temp: ");
  Serial.println(t);
  Serial.print("Raw DHT Humi: ");
  Serial.println(h);
  Serial.println("====================");
  // ===============================================

  Serial.println("=== Init Storage & Logger ===");
  if (!spiffsStorage.init()) {
    Serial.println("ERROR: SPIFFS initialization failed!");
  } else {
    Serial.println("SPIFFS initialized successfully");
    
    // 加载配置文件
    if (configManager.load()) {
      Serial.println("Configuration loaded successfully from SPIFFS");
    } else {
      Serial.println("No configuration file found, using defaults");
      configManager.resetToDefaults();
      configManager.save();
    }
  }
  dataLogger.init();
  
  Serial.println("=== Init Environment Sensors ===");
  environmentManager.addSensor(new TemperatureSensor("temperature", &dht, DHT_PIN, DHT11));
  environmentManager.addSensor(new HumiditySensor("humidity", &dht, DHT_PIN, DHT11));
  environmentManager.addSensor(new LightSensor("light", LIGHT_PIN));
  environmentManager.addSensor(new SmokeSensor("smoke", SMOKE_PIN));
  environmentManager.initializeAllSensors();
  Serial.println("=== Environment Sensors Ready ===");

  // ===== 配置烟雾告警 =====
  Serial.println("=== Init Alarm Manager ===");
  // 尝试从存储加载已有阈值
  bool loaded = alarmManager.loadFromStorage();
  AlarmThreshold smokeExisting = alarmManager.getThreshold("smoke");
  if (!loaded || (smokeExisting.maxThreshold == -1 && smokeExisting.minThreshold == -1)) {
    AlarmThreshold smokeThreshold;
    smokeThreshold.sensorId = "smoke";
    smokeThreshold.minThreshold = -1;   // 不设下限
    smokeThreshold.maxThreshold = configManager.getSmokeThreshold();  // 从配置中获取阈值
    smokeThreshold.enabled = true;
    alarmManager.updateThreshold(smokeThreshold);
  }
  alarmManager.setBuzzer("buzzer_001");

  // (可选) 配置联动：报警时自动打开开关(如排风扇)
  // AlarmAction turnOnFan;
  // turnOnFan.deviceId = "switch_001";
  // turnOnFan.targetStatus = true;
  // alarmManager.addAlarmAction(turnOnFan);

  // 联动配置 1：打开灯光 (视觉报警)
  

  // 联动配置 2：打开蜂鸣器 (听觉报警)
  // [禁用默认联动] 避免因为烟雾传感器浮空导致蜂鸣器一直响无法关闭
  // AlarmAction turnOnBuzzer;
  // turnOnBuzzer.deviceId = "buzzer_001";
  // turnOnBuzzer.targetStatus = true;
  // alarmManager.addAlarmAction(turnOnBuzzer);
  
  Serial.println("=== Alarm Manager Ready ===");

  if (!wiFiManager.connect()) {
    Serial.println("WiFi连接失败，但系统继续运行");
  }

  webServer.initialize();
  webServer.start();

  Serial.println("=== SETUP END ===");
}

// ===== Arduino loop =====
void loop() {
  wiFiManager.update();
  static unsigned long lastEnvCollect = 0;
  if (millis() - lastEnvCollect >= 2000) {
    lastEnvCollect = millis();
    environmentManager.collectAllSensorData();
    // 只有在数据采集后才检查告警
    std::vector<AlarmStatus> alarms = alarmManager.checkAlarms(environmentManager);
    
    

    // 只有当有告警触发时，才执行联动 (用于控制灯光等其他设备)
    if (!alarms.empty()) {
        alarmManager.executeAlarmActions(deviceManager);
    }
    if (alarmManager.syncBuzzer(deviceManager)) {
      Device* buzzer = deviceManager.getDevice("buzzer_001");
      if (buzzer) {
        webServer.notifyDeviceUpdate(buzzer);
      }
    }
  }
  ruleEngine.evaluateRules(deviceManager, environmentManager);
  // 移除每轮循环无条件执行 executeAlarmActions，避免它锁死设备状态
  // alarmManager.executeAlarmActions(deviceManager);
  delay(configManager.getMainLoopDelayMs());
}
