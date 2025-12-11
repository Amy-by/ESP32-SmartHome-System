// ESP32智能家居控制系统 - 基本设备控制示例
// 本示例展示如何使用DeviceManager控制智能灯和智能开关

#include <Arduino.h>
#include "src/devices/DeviceManager.h"
#include "src/devices/SmartLight.h"
#include "src/devices/SmartSwitch.h"
#include "src/utils/GPIOController.h"

// 创建设备管理器和GPIO控制器实例
GPIOController gpioController;
DeviceManager deviceManager(gpioController);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // 等待串口连接
  }
  
  Serial.println("ESP32智能家居控制系统 - 基本设备控制示例");
  
  // 初始化GPIO控制器
  if (!gpioController.initialize()) {
    Serial.println("GPIO控制器初始化失败");
    return;
  }
  
  // 添加智能灯设备
  deviceManager.addDevice(new SmartLight("light_1", "客厅灯", 13));
  
  // 添加智能开关设备
  deviceManager.addDevice(new SmartSwitch("switch_1", "插座", 12));
  
  Serial.println("设备添加完成");
}

void loop() {
  // 控制智能灯
  Serial.println("\n=== 控制智能灯 ===");
  
  // 打开智能灯
  deviceManager.controlDevice("light_1", true);
  Serial.println("客厅灯已打开");
  delay(2000);
  
  // 调节智能灯亮度
  SmartLight* light = static_cast<SmartLight*>(deviceManager.getDeviceById("light_1"));
  if (light) {
    light->setBrightness(50);
    Serial.println("客厅灯亮度已设置为50%");
  }
  delay(2000);
  
  // 关闭智能灯
  deviceManager.controlDevice("light_1", false);
  Serial.println("客厅灯已关闭");
  delay(2000);
  
  // 控制智能开关
  Serial.println("\n=== 控制智能开关 ===");
  
  // 打开智能开关
  deviceManager.controlDevice("switch_1", true);
  Serial.println("插座已打开");
  delay(2000);
  
  // 关闭智能开关
  deviceManager.controlDevice("switch_1", false);
  Serial.println("插座已关闭");
  delay(2000);
}
