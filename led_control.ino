#include <WiFi.h>
#include <ArduinoJson.h>
#include "src/devices/DeviceManager.h"
#include "src/devices/SmartLight.h"
#include "src/devices/SmartSwitch.h"
#include "src/sensors/EnvironmentManager.h"
#include "src/sensors/TemperatureSensor.h"
#include "src/sensors/HumiditySensor.h"
#include "src/sensors/LightSensor.h"
#include "src/sensors/SmokeSensor.h"
#include "src/utils/WiFiManager.h"
#include "src/web/WebServer.h"
#include "webpage.h" // 引入HTML网页内容

// WiFi配置
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

// 设备和传感器引脚定义
const int ledPin = 2; // ESP32板载LED
const int smartLightPin = 4;
const int smartSwitchPin = 5;
const int dhtPin = 14;
const int lightSensorPin = 32;
const int smokeSensorPin = 33;

// 创建管理器实例
DeviceManager deviceManager;
EnvironmentManager environmentManager;
WiFiManager wifiManager(ssid, password);
WebServer webServer(deviceManager, environmentManager, wifiManager);

void setup() {
    // 初始化串口
    Serial.begin(115200);
    
    // 连接WiFi
    Serial.print("正在连接WiFi...");
    if (wifiManager.connect()) {
        Serial.println("WiFi连接成功");
        Serial.print("IP地址: ");
        Serial.println(wifiManager.getIPAddress());
    } else {
        Serial.println("WiFi连接失败");
    }
    
    // 初始化设备
    initializeDevices();
    
    // 初始化传感器
    initializeSensors();
    
    // 初始化Web服务器
    webServer.initialize();
    webServer.start();
    Serial.println("Web服务器已启动");
}

void loop() {
    // 检查WiFi连接状态
    wifiManager.checkConnection();
    
    // 定期采集环境数据
    static unsigned long lastCollectionTime = 0;
    if (millis() - lastCollectionTime > 5000) { // 每5秒采集一次
        environmentManager.collectAllSensorData();
        lastCollectionTime = millis();
    }
    
    // 处理设备控制和状态更新
    // 这里可以添加自动控制逻辑
}

// 初始化设备
void initializeDevices() {
    // 创建智能灯
    SmartLight* livingRoomLight = new SmartLight("light_001", "客厅灯", smartLightPin);
    if (deviceManager.addDevice(livingRoomLight)) {
        Serial.println("客厅灯已添加");
    }
    
    // 创建智能开关
    SmartSwitch* fanSwitch = new SmartSwitch("switch_001", "风扇开关", smartSwitchPin);
    if (deviceManager.addDevice(fanSwitch)) {
        Serial.println("风扇开关已添加");
    }
    
    // 创建板载LED灯
    SmartLight* onboardLed = new SmartLight("light_002", "板载LED", ledPin);
    if (deviceManager.addDevice(onboardLed)) {
        Serial.println("板载LED已添加");
    }
}

// 初始化传感器
void initializeSensors() {
    // 创建温度传感器
    TemperatureSensor* temperatureSensor = new TemperatureSensor("temp_001", "温度传感器", dhtPin);
    if (environmentManager.addSensor(temperatureSensor)) {
        Serial.println("温度传感器已添加");
    }
    
    // 创建湿度传感器
    HumiditySensor* humiditySensor = new HumiditySensor("hum_001", "湿度传感器", dhtPin);
    if (environmentManager.addSensor(humiditySensor)) {
        Serial.println("湿度传感器已添加");
    }
    
    // 创建光照传感器
    LightSensor* lightSensor = new LightSensor("light_001", "光照传感器", lightSensorPin);
    if (environmentManager.addSensor(lightSensor)) {
        Serial.println("光照传感器已添加");
    }
    
    // 创建烟雾传感器
    SmokeSensor* smokeSensor = new SmokeSensor("smoke_001", "烟雾传感器", smokeSensorPin);
    if (environmentManager.addSensor(smokeSensor)) {
        Serial.println("烟雾传感器已添加");
    }
    
    // 初始化所有传感器
    if (environmentManager.initializeAllSensors()) {
        Serial.println("所有传感器初始化成功");
    } else {
        Serial.println("传感器初始化失败");
    }
}