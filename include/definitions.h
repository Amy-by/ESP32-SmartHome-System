#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <Arduino.h>

// 设备类型定义
#define DEVICE_TYPE_LIGHT "light"
#define DEVICE_TYPE_SWITCH "switch"
#define DEVICE_TYPE_BUZZER "buzzer"

// 传感器类型定义
#define SENSOR_TYPE_TEMPERATURE "temperature"
#define SENSOR_TYPE_HUMIDITY "humidity"
#define SENSOR_TYPE_LIGHT "light"
#define SENSOR_TYPE_SMOKE "smoke"

// 告警级别定义
#define ALARM_LEVEL_INFO "INFO"
#define ALARM_LEVEL_WARNING "WARNING"
#define ALARM_LEVEL_ERROR "ERROR"
#define ALARM_LEVEL_CRITICAL "CRITICAL"

// 日志级别定义
#define LOG_LEVEL_DEBUG "DEBUG"
#define LOG_LEVEL_INFO "INFO"
#define LOG_LEVEL_WARNING "WARNING"
#define LOG_LEVEL_ERROR "ERROR"
#define LOG_LEVEL_ALARM "ALARM"

// 设备状态定义
#define DEVICE_STATUS_ON true
#define DEVICE_STATUS_OFF false

// SPIFFS文件路径定义
#define SPIFFS_CONFIG_FILE "/config.json"
#define SPIFFS_LOG_FILE "/logs.txt"
#define SPIFFS_WEB_INDEX "/index.html"
#define SPIFFS_WEB_STYLES "/styles.css"
#define SPIFFS_WEB_SCRIPTS "/scripts.js"

#endif // DEFINITIONS_H
