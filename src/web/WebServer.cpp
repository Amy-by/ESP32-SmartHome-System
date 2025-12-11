// WebServer.cpp - Web服务器类实现文件
// 实现ESP32智能家居控制系统的Web服务器功能

#include "WebServer.h"
#include "webpage.h"

// 构造函数
WebServer::WebServer(DeviceManager& deviceManager, EnvironmentManager& environmentManager, WiFiManager& wiFiManager)
    : server(80),
      ws("/ws"),
      deviceManager(deviceManager),
      environmentManager(environmentManager),
      wiFiManager(wiFiManager),
      running(false),
      htmlContent(webpageContent) // 从webpage.h导入HTML内容
{
    // 验证HTML内容是否正确加载
    if (htmlContent == nullptr) {
        Serial.println("警告: HTML内容未正确加载");
    }
}

// 析构函数
WebServer::~WebServer() {
    stop();
}

// 初始化Web服务器
void WebServer::initialize() {
    // 配置API路由
    setupApiRoutes();
    
    // 配置网页路由
    setupWebRoutes();
    
    // 配置WebSocket路由
    setupWebSocketRoutes();
}

// 启动Web服务器
void WebServer::start() {
    server.begin();
    running = true;
}

// 停止Web服务器
void WebServer::stop() {
    server.end();
    running = false;
}

// 获取服务器状态
bool WebServer::isRunning() const {
    return running;
}

// 配置API路由
void WebServer::setupApiRoutes() {
    // 获取所有设备状态
    server.on("/api/devices", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetDevices(request);
    });
    
    // 获取单个设备状态
    server.on("/api/devices/", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetDevice(request);
    });
    
    // 控制设备
    server.on("/api/devices/control", HTTP_POST, [this](AsyncWebServerRequest* request) {}, 
             nullptr, [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, data, len);
        handleControlDevice(request, doc.as<JsonVariant>());
    });
    
    // 获取环境数据
    server.on("/api/environment", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetEnvironmentData(request);
    });
    
    // 获取WiFi状态
    server.on("/api/wifi", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetWiFiStatus(request);
    });
}

// 配置网页路由
void WebServer::setupWebRoutes() {
    // 主页
    server.on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
        // 替换占位符为实际内容
        String content = webpageContent;
        content.replace("%STYLES%", stylesContent);
        content.replace("%SCRIPTS%", scriptsContent);
        request->send(200, "text/html", content);
    });
    
    // 提供静态文件
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(404);
    });
}

// 配置WebSocket路由
void WebServer::setupWebSocketRoutes() {
    ws.onEvent(onWebSocketEvent);
    server.addHandler(&ws);
}

// 处理设备状态API请求
void WebServer::handleGetDevices(AsyncWebServerRequest* request) {
    DynamicJsonDocument doc(2048);
    JsonArray devices = doc.createNestedArray("devices");
    
    // 获取所有设备
    std::vector<Device*> deviceList = deviceManager.getAllDevices();
    
    // 遍历设备列表，添加设备信息到JSON
    for (Device* device : deviceList) {
        JsonObject deviceJson = devices.createNestedObject();
        deviceJson["id"] = device->getId();
        deviceJson["name"] = device->getName();
        deviceJson["type"] = device->getType();
        deviceJson["status"] = device->getStatus();
        
        // 如果是智能灯，添加亮度信息
        if (device->getType() == "SmartLight") {
            SmartLight* light = static_cast<SmartLight*>(device);
            deviceJson["brightness"] = light->getBrightness();
        }
    }
    
    doc["status"] = "success";
    doc["message"] = "获取设备状态成功";
    
    sendJsonResponse(request, doc);
}

// 处理单个设备状态API请求
void WebServer::handleGetDevice(AsyncWebServerRequest* request) {
    if (request->url().length() <= strlen("/api/devices/")) {
        request->send(400, "text/plain", "缺少设备ID");
        return;
    }
    
    // 提取设备ID
    String deviceId = request->url().substring(strlen("/api/devices/"));
    
    // 获取设备
    Device* device = deviceManager.getDeviceById(deviceId);
    
    if (!device) {
        request->send(404, "text/plain", "设备不存在");
        return;
    }
    
    DynamicJsonDocument doc(1024);
    JsonObject deviceJson = doc.createNestedObject("device");
    deviceJson["id"] = device->getId();
    deviceJson["name"] = device->getName();
    deviceJson["type"] = device->getType();
    deviceJson["status"] = device->getStatus();
    
    // 如果是智能灯，添加亮度信息
    if (device->getType() == "SmartLight") {
        SmartLight* light = static_cast<SmartLight*>(device);
        deviceJson["brightness"] = light->getBrightness();
    }
    
    doc["status"] = "success";
    doc["message"] = "获取设备状态成功";
    
    sendJsonResponse(request, doc);
}

// 处理设备控制API请求
void WebServer::handleControlDevice(AsyncWebServerRequest* request, JsonVariant& json) {
    String deviceId = json["id"].as<String>();
    bool status = json["status"].as<bool>();
    int brightness = json["brightness"].as<int>();
    
    // 获取设备
    Device* device = deviceManager.getDeviceById(deviceId);
    
    if (!device) {
        request->send(404, "text/plain", "设备不存在");
        return;
    }
    
    // 控制设备
    bool result = false;
    if (device->getType() == "SmartLight") {
        SmartLight* light = static_cast<SmartLight*>(device);
        if (brightness >= 0 && brightness <= 100) {
            result = light->setBrightness(brightness);
        } else {
            result = light->setState(status);
        }
    } else if (device->getType() == "SmartSwitch") {
        SmartSwitch* switchDevice = static_cast<SmartSwitch*>(device);
        result = switchDevice->setState(status);
    }
    
    if (result) {
        DynamicJsonDocument doc(256);
        doc["status"] = "success";
        doc["message"] = "设备控制成功";
        sendJsonResponse(request, doc);
    } else {
        request->send(500, "text/plain", "设备控制失败");
    }
}

// 处理环境数据API请求
void WebServer::handleGetEnvironmentData(AsyncWebServerRequest* request) {
    DynamicJsonDocument doc(1024);
    JsonObject environment = doc.createNestedObject("environment");
    
    // 获取温度数据
    float temperature = environmentManager.getTemperature();
    if (temperature != -999.0) {
        environment["temperature"] = temperature;
    }
    
    // 获取湿度数据
    float humidity = environmentManager.getHumidity();
    if (humidity != -999.0) {
        environment["humidity"] = humidity;
    }
    
    // 获取光照强度数据
    float lightIntensity = environmentManager.getLightIntensity();
    if (lightIntensity != -999.0) {
        environment["lightIntensity"] = lightIntensity;
    }
    
    // 获取烟雾浓度数据
    float smokeDensity = environmentManager.getSmokeDensity();
    if (smokeDensity != -999.0) {
        environment["smokeDensity"] = smokeDensity;
    }
    
    doc["status"] = "success";
    doc["message"] = "获取环境数据成功";
    
    sendJsonResponse(request, doc);
}

// 处理WiFi状态API请求
void WebServer::handleGetWiFiStatus(AsyncWebServerRequest* request) {
    DynamicJsonDocument doc(256);
    JsonObject wifi = doc.createNestedObject("wifi");
    
    wifi["connected"] = wiFiManager.isConnected();
    wifi["ssid"] = wiFiManager.getSSID();
    wifi["ip"] = wiFiManager.getIPAddress();
    wifi["rssi"] = wiFiManager.getRSSI();
    
    doc["status"] = "success";
    doc["message"] = "获取WiFi状态成功";
    
    sendJsonResponse(request, doc);
}

// 发送JSON响应
void WebServer::sendJsonResponse(AsyncWebServerRequest* request, DynamicJsonDocument& doc, int code) {
    String jsonString;
    serializeJson(doc, jsonString);
    request->send(code, "application/json", jsonString);
}

// WebSocket事件处理
void WebServer::onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type,
                                void* arg, uint8_t* data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
    } else if (type == WS_EVT_DATA) {
        // 处理WebSocket数据
        AwsFrameInfo* info = (AwsFrameInfo*)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            // 解析JSON数据
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, data, len);
            
            // 根据消息类型处理
            String type = doc["type"].as<String>();
            if (type == "ping") {
                // 响应ping请求
                DynamicJsonDocument response(256);
                response["type"] = "pong";
                String responseString;
                serializeJson(response, responseString);
                client->text(responseString);
            }
        }
    }
}
