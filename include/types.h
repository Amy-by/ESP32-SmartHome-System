#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>
#include <ArduinoJson.h>

// 设备状态结构体
typedef struct {
    String deviceId;
    String deviceType;
    bool status;
    int brightness;  // 仅用于灯光设备
    int pin;
} DeviceState;

// 环境数据结构体
typedef struct {
    String sensorType;
    float value;
    String unit;
    unsigned long timestamp;
} EnvironmentData;

// 规则条件结构体
typedef struct {
    String sensorType;
    String op;  // >, <, ==, !=, >=, <=
    float threshold;
} RuleCondition;

// 规则动作结构体
typedef struct {
    String deviceId;
    String actionType;  // on, off, set_brightness
    int value;  // 用于设置亮度等
} RuleAction;

// 规则结构体
typedef struct {
    String ruleId;
    String ruleName;
    bool enabled;
    RuleCondition condition;
    RuleAction action;
    String description;
} Rule;

// 告警设置结构体
typedef struct {
    String alarmType;
    float threshold;
    bool enabled;
    int duration;  // 告警持续时间（秒）
    String deviceId;  // 告警设备ID
} AlarmSetting;

// WiFi配置结构体
typedef struct {
    String ssid;
    String password;
    bool dhcpEnabled;
    IPAddress staticIP;
    IPAddress gateway;
    IPAddress subnetMask;
    IPAddress dnsServer;
} WiFiConfig;

// 系统配置结构体
typedef struct {
    WiFiConfig wifiConfig;
    int sensorUpdateInterval;  // 传感器更新间隔（毫秒）
    int devicePollingInterval;  // 设备轮询间隔（毫秒）
    int maxLogSize;  // 最大日志大小（字节）
    bool debugMode;  // 是否启用调试模式
} SystemConfig;

#endif // TYPES_H
