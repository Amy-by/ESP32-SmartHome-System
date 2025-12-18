#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// WiFi配置参数（默认值，可通过配置文件或Web界面修改）
#define DEFAULT_WIFI_SSID "Redmi Note 11 Pro"
#define DEFAULT_WIFI_PASSWORD "10098765432yU"
#define DEFAULT_WIFI_DHCP_ENABLED true

// 静态IP配置（仅当DHCP_DISABLED为true时使用）
#define DEFAULT_STATIC_IP IPAddress(192, 168, 1, 100)
#define DEFAULT_GATEWAY IPAddress(192, 168, 1, 1)
#define DEFAULT_SUBNET_MASK IPAddress(255, 255, 255, 0)
#define DEFAULT_DNS_SERVER IPAddress(8, 8, 8, 8)

// 传感器配置参数
#define DEFAULT_SENSOR_UPDATE_INTERVAL 1000  // 传感器更新间隔（毫秒）
#define DEFAULT_TEMPERATURE_THRESHOLD 28.0  // 温度告警阈值（℃）
#define DEFAULT_HUMIDITY_THRESHOLD 70.0      // 湿度告警阈值（%）
#define DEFAULT_LIGHT_THRESHOLD 500          // 光照告警阈值（lx）
#define DEFAULT_SMOKE_THRESHOLD 500          // 烟雾浓度告警阈值（ppm）

// 设备配置参数
#define DEFAULT_DEVICE_POLLING_INTERVAL 500  // 设备轮询间隔（毫秒）

// 存储配置参数
#define DEFAULT_MAX_LOG_SIZE 102400  // 最大日志大小（字节）- 100KB
#define SPIFFS_MAX_FILE_SIZE 4096     // SPIFFS文件最大大小（字节）

// Web服务器配置参数
#define DEFAULT_WEB_SERVER_PORT 80  // Web服务器端口

// 主循环与定期保存配置
#define MAIN_LOOP_DELAY_MS 100       // 主循环延迟（毫秒）
#define SAVE_INTERVAL_MS 60000       // 定期保存间隔（毫秒）

// 告警配置参数
#define DEFAULT_ALARM_DURATION 30  // 告警持续时间（秒）
#define DEFAULT_ALARM_ENABLED true  // 默认启用告警

// 调试配置
#define DEFAULT_DEBUG_MODE false  // 默认关闭调试模式

#endif // CONFIG_H
