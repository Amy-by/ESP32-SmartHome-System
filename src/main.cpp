// main.cpp - ESP32智能家居控制系统主程序入口
// 负责初始化各个模块并启动系统

#include <Arduino.h>
#include "devices/DeviceManager.h"
#include "sensors/EnvironmentManager.h"
#include "core/RuleEngine.h"
#include "core/AlarmManager.h"
#include "utils/WiFiManager.h"
#include "utils/GPIOController.h"
#include "web/WebServer.h"
#include "utils/DataLogger.h"
#include "utils/EEPROMStorage.h"
#include "utils/SPIFFSStorage.h"
#include "utils/DataProcessor.h"
#include "include/config.h"
#include "include/definitions.h"
#include "include/types.h"

// 全局对象声明
WiFiManager wiFiManager;
GPIOController gpioController;
DataProcessor dataProcessor;
EEPROMStorage eepromStorage;
SPIFFSStorage spiffsStorage;
DataLogger dataLogger(eepromStorage, spiffsStorage);
DeviceManager deviceManager(gpioController, eepromStorage, dataLogger);
EnvironmentManager environmentManager(gpioController, dataProcessor, dataLogger);
RuleEngine ruleEngine(deviceManager, environmentManager, eepromStorage);
AlarmManager alarmManager(deviceManager, environmentManager, gpioController, dataLogger, eepromStorage);
WebServer webServer(deviceManager, environmentManager, wiFiManager);

// 系统状态变量
bool systemInitialized = false;

/**
 * @brief 初始化系统
 */
void initializeSystem() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== ESP32智能家居控制系统启动 ===");
  
  // 1. 初始化存储
  Serial.println("1. 初始化存储系统...");
  if (eepromStorage.initialize()) {
    Serial.println("   EEPROM存储初始化成功");
  } else {
    Serial.println("   EEPROM存储初始化失败");
  }
  
  if (spiffsStorage.initialize()) {
    Serial.println("   SPIFFS存储初始化成功");
  } else {
    Serial.println("   SPIFFS存储初始化失败");
  }
  
  // 2. 初始化GPIO控制器
  Serial.println("2. 初始化GPIO控制器...");
  gpioController.initialize();
  Serial.println("   GPIO控制器初始化成功");
  
  // 3. 初始化设备管理器
  Serial.println("3. 初始化设备管理器...");
  if (deviceManager.initialize()) {
    Serial.println("   设备管理器初始化成功");
  } else {
    Serial.println("   设备管理器初始化失败");
  }
  
  // 4. 初始化环境管理器
  Serial.println("4. 初始化环境管理器...");
  if (environmentManager.initialize()) {
    Serial.println("   环境管理器初始化成功");
  } else {
    Serial.println("   环境管理器初始化失败");
  }
  
  // 5. 初始化WiFi连接
  Serial.println("5. 连接WiFi网络...");
  if (wiFiManager.connect()) {
    Serial.print("   WiFi连接成功，IP地址: ");
    Serial.println(wiFiManager.getIPAddress());
    
    // 6. 初始化Web服务器
    Serial.println("6. 初始化Web服务器...");
    webServer.initialize();
    webServer.start();
    Serial.println("   Web服务器初始化成功");
  } else {
    Serial.println("   WiFi连接失败，将继续运行本地功能");
  }
  
  // 7. 初始化规则引擎
  Serial.println("7. 初始化规则引擎...");
  if (ruleEngine.initialize()) {
    Serial.println("   规则引擎初始化成功");
  } else {
    Serial.println("   规则引擎初始化失败");
  }
  
  // 8. 初始化告警管理器
  Serial.println("8. 初始化告警管理器...");
  if (alarmManager.initialize()) {
    Serial.println("   告警管理器初始化成功");
  } else {
    Serial.println("   告警管理器初始化失败");
  }
  
  systemInitialized = true;
  Serial.println("\n=== 系统初始化完成 ===");
}

/**
 * @brief 主循环函数
 */
void loop() {
  // 检查系统是否初始化成功
  if (!systemInitialized) {
    initializeSystem();
    return;
  }
  
  // 更新WiFi状态
  wiFiManager.update();
  
  // 采集环境数据
  environmentManager.update();
  
  // 处理规则
  ruleEngine.evaluateRules();
  
  // 检查告警
  alarmManager.checkAlarms();
  
  // 更新设备状态
  deviceManager.update();
  
  // 定期保存数据
  static unsigned long lastSaveTime = 0;
  if (millis() - lastSaveTime > SAVE_INTERVAL_MS) {
    deviceManager.saveDeviceStates();
    environmentManager.saveSensorData();
    lastSaveTime = millis();
  }
  
  // 系统延迟
  delay(MAIN_LOOP_DELAY_MS);
}

/**
 * @brief Arduino setup函数
 */
void setup() {
  initializeSystem();
}