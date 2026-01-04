// WebServer.h - Web服务器类头文件
// 实现ESP32智能家居控制系统的Web服务器功能，包括API接口和网页界面

#ifndef SMART_HOME_WEBSERVER_H
#define SMART_HOME_WEBSERVER_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "../devices/DeviceManager.h"
#include "../sensors/EnvironmentManager.h"
#include "../utils/WiFiManager.h"
#include "../core/AlarmManager.h"
#include "../utils/SPIFFSStorage.h"
#include "../utils/ConfigManager.h"
#include "../utils/DatabaseManager.h"
#include "../core/RuleEngine.h"

class WebServer {
public:
    /**
     * @brief WebServer构造函数
     * @param deviceManager 设备管理器实例
     * @param environmentManager 环境管理器实例
     * @param wiFiManager WiFi管理器实例
     * @param alarmManager 告警管理器实例
     * @param spiffsStorage SPIFFS存储实例
     * @param configManager 配置管理器实例
     * @param databaseManager 数据库管理器实例
     * @param ruleEngine 规则引擎实例
     */
    WebServer(DeviceManager& deviceManager, EnvironmentManager& environmentManager, WiFiManager& wiFiManager, AlarmManager& alarmManager, SPIFFSStorage& spiffsStorage, ConfigManager& configManager, DatabaseManager& databaseManager, RuleEngine& ruleEngine);
    
    /**
     * @brief WebServer析构函数
     */
    ~WebServer();
    
    /**
     * @brief 初始化Web服务器
     */
    void initialize();
    
    /**
     * @brief 启动Web服务器
     */
    void start();
    
    /**
     * @brief 停止Web服务器
     */
    void stop();
    
    /**
     * @brief 获取服务器状态
     * @return true 服务器正在运行，false 服务器已停止
     */
    bool isRunning() const;
    
    void notifyDeviceUpdate(Device* device);
    
private:
    AsyncWebServer server;              // Web服务器实例
    AsyncWebSocket ws;                  // WebSocket实例
    DeviceManager& deviceManager;       // 设备管理器引用
    EnvironmentManager& environmentManager;  // 环境管理器引用
    WiFiManager& wiFiManager;           // WiFi管理器引用
    AlarmManager& alarmManager;         // 告警管理器引用
    SPIFFSStorage& spiffsStorage;       // SPIFFS存储引用
    ConfigManager& configManager;       // 配置管理器引用
    DatabaseManager& databaseManager;   // 数据库管理器引用
    RuleEngine& ruleEngine;             // 规则引擎引用
    bool running;                       // 服务器运行状态
    
    /**
     * @brief 配置API路由
     */
    void setupApiRoutes();
    
    /**
     * @brief 配置网页路由
     */
    void setupWebRoutes();
    
    /**
     * @brief 配置WebSocket路由
     */
    void setupWebSocketRoutes();
    
    /**
     * @brief 处理设备状态API请求
     * @param request Web请求
     */
    void handleGetDevices(AsyncWebServerRequest* request);
    
    /**
     * @brief 处理单个设备状态API请求
     * @param request Web请求
     */
    void handleGetDevice(AsyncWebServerRequest* request);
    
    /**
     * @brief 处理设备控制API请求
     * @param request Web请求
     */
    void handleControlDevice(AsyncWebServerRequest* request, const JsonVariantConst& json);
    
    /**
     * @brief 处理环境数据API请求
     * @param request Web请求
     */
    void handleGetEnvironmentData(AsyncWebServerRequest* request);
    
    /**
     * @brief 处理WiFi状态API请求
     * @param request Web请求
     */
    void handleGetWiFiStatus(AsyncWebServerRequest* request);
    
    /**
     * @brief 获取告警阈值设置
     */
    void handleGetAlarmSettings(AsyncWebServerRequest* request);
    
    /**
     * @brief 更新告警阈值设置
     */
    void handleUpdateAlarmSettings(AsyncWebServerRequest* request, const JsonVariantConst& json);
    
    /**
     * @brief 获取完整配置
     * @param request Web请求
     */
    void handleGetConfig(AsyncWebServerRequest* request);
    
    /**
     * @brief 更新配置
     * @param request Web请求
     * @param json JSON配置数据
     */
    void handleUpdateConfig(AsyncWebServerRequest* request, const JsonVariantConst& json);
    
    /**
     * @brief 处理管理员登录请求
     * @param request Web请求
     * @param json JSON登录数据
     */
    void handleAdminLogin(AsyncWebServerRequest* request, const JsonVariantConst& json);
    
    /**
     * @brief 处理创建设备API请求
     * @param request Web请求
     * @param json JSON设备数据
     */
    void handleCreateDevice(AsyncWebServerRequest* request, const JsonVariantConst& json);
    
    /**
     * @brief 处理删除设备API请求
     * @param request Web请求
     */
    void handleDeleteDevice(AsyncWebServerRequest* request);
    
    /**
     * @brief 处理编辑设备API请求
     * @param request Web请求
     * @param json JSON设备数据
     */
    void handleEditDevice(AsyncWebServerRequest* request, const JsonVariantConst& json);
    
    /**
     * @brief 处理获取传感器数据API请求
     * @param request Web请求
     */
    void handleGetSensorData(AsyncWebServerRequest* request);
    
    /**
     * @brief 处理创建规则API请求
     * @param request Web请求
     * @param json JSON规则数据
     */
    void handleCreateRule(AsyncWebServerRequest* request, const JsonVariantConst& json);
    
    /**
     * @brief 处理获取所有规则API请求
     * @param request Web请求
     */
    void handleGetRules(AsyncWebServerRequest* request);
    
    /**
     * @brief 处理更新规则API请求
     * @param request Web请求
     * @param json JSON规则数据
     */
    void handleUpdateRule(AsyncWebServerRequest* request, const JsonVariantConst& json);
    
    /**
     * @brief 处理删除规则API请求
     * @param request Web请求
     */
    void handleDeleteRule(AsyncWebServerRequest* request);
    
    /**
     * @brief 发送JSON响应
     * @param request Web请求
     * @param doc JSON文档
     * @param code HTTP状态码
     */
    void sendJsonResponse(AsyncWebServerRequest* request, DynamicJsonDocument& doc, int code = 200);
    
    /**
     * @brief 通过WebSocket广播设备状态更新
     * @param device 已更新的设备指针
     */
    void broadcastDeviceUpdate(Device* device);
    
    /**
     * @brief WebSocket事件处理
     * @param server WebSocket服务器
     * @param client WebSocket客户端
     * @param type 事件类型
     * @param arg 事件参数
     * @param data 事件数据
     * @param len 数据长度
     */
    static void onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type,
                                void* arg, uint8_t* data, size_t len);
};

#endif /* SMART_HOME_WEBSERVER_H */
