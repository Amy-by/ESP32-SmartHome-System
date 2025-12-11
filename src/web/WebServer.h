// WebServer.h - Web服务器类头文件
// 实现ESP32智能家居控制系统的Web服务器功能，包括API接口和网页界面

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "../devices/DeviceManager.h"
#include "../sensors/EnvironmentManager.h"
#include "../utils/WiFiManager.h"

class WebServer {
public:
    /**
     * @brief WebServer构造函数
     * @param deviceManager 设备管理器实例
     * @param environmentManager 环境管理器实例
     * @param wiFiManager WiFi管理器实例
     */
    WebServer(DeviceManager& deviceManager, EnvironmentManager& environmentManager, WiFiManager& wiFiManager);
    
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
    
private:
    AsyncWebServer server;              // Web服务器实例
    AsyncWebSocket ws;                  // WebSocket实例
    DeviceManager& deviceManager;       // 设备管理器引用
    EnvironmentManager& environmentManager;  // 环境管理器引用
    WiFiManager& wiFiManager;           // WiFi管理器引用
    bool running;                       // 服务器运行状态
    
    // HTML网页内容
    const char* htmlContent;
    
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
    void handleControlDevice(AsyncWebServerRequest* request, JsonVariant& json);
    
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
     * @brief 发送JSON响应
     * @param request Web请求
     * @param doc JSON文档
     * @param code HTTP状态码
     */
    void sendJsonResponse(AsyncWebServerRequest* request, DynamicJsonDocument& doc, int code = 200);
    
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

#endif /* WEBSERVER_H */
