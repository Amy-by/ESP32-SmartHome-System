// ESP32智能家居控制系统 - 高级自动化示例
// 本示例展示如何使用规则引擎实现基于环境数据的自动化控制

#include <Arduino.h>
#include "src/devices/DeviceManager.h"
#include "src/devices/SmartLight.h"
#include "src/devices/SmartSwitch.h"
#include "src/sensors/EnvironmentManager.h"
#include "src/sensors/TemperatureSensor.h"
#include "src/sensors/LightSensor.h"
#include "src/core/RuleEngine.h"
#include "src/core/Rule.h"
#include "src/utils/GPIOController.h"

// 创建设备管理器和GPIO控制器实例
GPIOController gpioController;
DeviceManager deviceManager(gpioController);
EnvironmentManager environmentManager;
RuleEngine ruleEngine(deviceManager, environmentManager);

// 传感器实例
TemperatureSensor temperatureSensor("temp_sensor", "温度传感器", 34);
LightSensor lightSensor("light_sensor", "光照传感器", 35);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // 等待串口连接
  }
  
  Serial.println("ESP32智能家居控制系统 - 高级自动化示例");
  
  // 初始化GPIO控制器
  if (!gpioController.initialize()) {
    Serial.println("GPIO控制器初始化失败");
    return;
  }
  
  // 添加智能灯设备
  deviceManager.addDevice(new SmartLight("light_1", "客厅灯", 13));
  deviceManager.addDevice(new SmartLight("light_2", "卧室灯", 14));
  
  // 添加智能开关设备
  deviceManager.addDevice(new SmartSwitch("switch_1", "空调插座", 12));
  
  // 初始化环境管理器
  environmentManager.initialize();
  
  // 添加环境传感器
  environmentManager.addSensor(&temperatureSensor);
  environmentManager.addSensor(&lightSensor);
  
  Serial.println("设备和传感器添加完成");
  
  // 创建自动化规则
  createAutomationRules();
  
  Serial.println("自动化规则创建完成");
}

void loop() {
  // 采集环境数据
  environmentManager.collectData();
  
  // 执行规则引擎
  ruleEngine.executeRules();
  
  // 打印当前状态
  printCurrentState();
  
  delay(5000); // 每5秒执行一次
}

void createAutomationRules() {
  // 规则1: 当光照强度低于500时，自动打开客厅灯
  Rule rule1("rule_1", "自动开灯");
  rule1.addCondition("light_sensor", RuleCondition::Operator::LESS_THAN, 500.0f);
  rule1.addAction("light_1", RuleAction::ActionType::TURN_ON);
  ruleEngine.addRule(rule1);
  
  // 规则2: 当光照强度高于1000时，自动关闭客厅灯
  Rule rule2("rule_2", "自动关灯");
  rule2.addCondition("light_sensor", RuleCondition::Operator::GREATER_THAN, 1000.0f);
  rule2.addAction("light_1", RuleAction::ActionType::TURN_OFF);
  ruleEngine.addRule(rule2);
  
  // 规则3: 当温度高于28度时，自动打开空调插座
  Rule rule3("rule_3", "自动开空调");
  rule3.addCondition("temp_sensor", RuleCondition::Operator::GREATER_THAN, 28.0f);
  rule3.addAction("switch_1", RuleAction::ActionType::TURN_ON);
  ruleEngine.addRule(rule3);
  
  // 规则4: 当温度低于24度时，自动关闭空调插座
  Rule rule4("rule_4", "自动关空调");
  rule4.addCondition("temp_sensor", RuleCondition::Operator::LESS_THAN, 24.0f);
  rule4.addAction("switch_1", RuleAction::ActionType::TURN_OFF);
  ruleEngine.addRule(rule4);
}

void printCurrentState() {
  Serial.println("\n=== 当前系统状态 ===");
  
  // 打印环境数据
  float temperature = environmentManager.getTemperature();
  float lightIntensity = environmentManager.getLightIntensity();
  
  Serial.print("温度: ");
  Serial.print(temperature);
  Serial.println(" °C");
  
  Serial.print("光照强度: ");
  Serial.print(lightIntensity);
  Serial.println(" lux");
  
  // 打印设备状态
  std::vector<Device*> devices = deviceManager.getAllDevices();
  for (Device* device : devices) {
    Serial.print(device->getName());
    Serial.print(" (" + device->getId() + "): ");
    Serial.println(device->getStatus() ? "开启" : "关闭");
    
    if (device->getType() == "SmartLight") {
      SmartLight* light = static_cast<SmartLight*>(device);
      Serial.print("  亮度: ");
      Serial.print(light->getBrightness());
      Serial.println("%");
    }
  }
}
