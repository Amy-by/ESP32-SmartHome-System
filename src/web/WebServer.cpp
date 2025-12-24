// WebServer.cpp - Web服务器类实现文件
// 实现ESP32智能家居控制系统的Web服务器功能

#include "WebServer.h"
#include "../devices/SmartLight.h"
#include "../devices/SmartSwitch.h"
#include "../core/AlarmManager.h"

// 构造函数
WebServer::WebServer(DeviceManager& deviceManager, EnvironmentManager& environmentManager, WiFiManager& wiFiManager, AlarmManager& alarmManager, SPIFFSStorage& spiffsStorage, ConfigManager& configManager)
    : server(80),
      ws("/ws"),
      deviceManager(deviceManager),
      environmentManager(environmentManager),
      wiFiManager(wiFiManager),
      alarmManager(alarmManager),
      spiffsStorage(spiffsStorage),
      configManager(configManager),
      running(false)
{
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

void WebServer::notifyDeviceUpdate(Device* device) {
    broadcastDeviceUpdate(device);
}

// 配置API路由
void WebServer::setupApiRoutes() {
    // 获取所有设备状态
    server.on("/api/devices", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetDevices(request);
    });
    server.on("/api/device", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetDevice(request);
    });
    
    // 控制设备
    server.on("/api/devices/control", HTTP_POST, [this](AsyncWebServerRequest* request) {}, 
             nullptr, [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, data, len);
        handleControlDevice(request, doc.as<JsonVariantConst>());
    });
    
    // 获取环境数据
    server.on("/api/environment", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetEnvironmentData(request);
    });
    
    // 获取WiFi状态
    server.on("/api/wifi", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetWiFiStatus(request);
    });
    
    // 获取告警阈值
    server.on("/api/alarm", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetAlarmSettings(request);
    });
    // 更新告警阈值（支持PUT与POST）
    server.on("/api/alarm", HTTP_PUT, [this](AsyncWebServerRequest* request) {}, 
             nullptr, [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
        DynamicJsonDocument doc(512);
        deserializeJson(doc, data, len);
        handleUpdateAlarmSettings(request, doc.as<JsonVariantConst>());
    });
    server.on("/api/alarm", HTTP_POST, [this](AsyncWebServerRequest* request) {}, 
             nullptr, [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
        DynamicJsonDocument doc(512);
        deserializeJson(doc, data, len);
        handleUpdateAlarmSettings(request, doc.as<JsonVariantConst>());
    });
    
    // 配置API路由
    server.on("/api/config", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetConfig(request);
    });
    server.on("/api/config", HTTP_PUT, [this](AsyncWebServerRequest* request) {}, 
             nullptr, [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
        DynamicJsonDocument doc(4096);
        deserializeJson(doc, data, len);
        handleUpdateConfig(request, doc.as<JsonVariantConst>());
    });
    server.on("/api/config", HTTP_POST, [this](AsyncWebServerRequest* request) {}, 
             nullptr, [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
        DynamicJsonDocument doc(4096);
        deserializeJson(doc, data, len);
        handleUpdateConfig(request, doc.as<JsonVariantConst>());
    });
}

// 配置网页路由
void WebServer::setupWebRoutes() {
    // 主页路由，从SPIFFS中提供index.html
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(SPIFFS, "/index.html", "text/html; charset=utf-8");
    });
    
    // 静态资源路由，从SPIFFS中提供文件
    server.serveStatic("/", SPIFFS, "/")
        .setDefaultFile("index.html");
}

// 配置WebSocket路由
void WebServer::setupWebSocketRoutes() {
    ws.onEvent(onWebSocketEvent);
    server.addHandler(&ws);
}

// 处理设备状态API请求
void WebServer::handleGetDevices(AsyncWebServerRequest* request) {
    Serial.print("Device count in WebServer: ");
    Serial.println(deviceManager.getAllDevices().size());
    DynamicJsonDocument doc(2048);
    JsonArray devices = doc.createNestedArray("devices");
    
    // 获取所有设备
    std::vector<Device*> deviceList = deviceManager.getAllDevices();
    
    // 遍历设备列表，添加设备信息到JSON
    for (Device* device : deviceList) {
        JsonObject deviceJson = devices.createNestedObject();
        deviceJson["id"] = device->getId();
        deviceJson["name"] = device->getId();
        deviceJson["type"] = device->getType();
        deviceJson["status"] = device->getStatus();
        
        if (device->getType() == "light") {
            SmartLight* light = static_cast<SmartLight*>(device);
            deviceJson["brightness"] = (int)(light->getBrightness() * 100 / 255);
        }
    }
    
    doc["status"] = "success";
    doc["message"] = "获取设备状态成功";
    
    sendJsonResponse(request, doc);
}

// 处理单个设备状态API请求
void WebServer::handleGetDevice(AsyncWebServerRequest* request) {
    String deviceId;
    if (request->hasParam("id")) {
        deviceId = request->getParam("id")->value();
    } else {
        request->send(400, "text/plain", "缺少设备ID");
        return;
    }
    
    // 获取设备
    Device* device = deviceManager.getDevice(deviceId);
    
    if (!device) {
        request->send(404, "text/plain", "设备不存在");
        return;
    }
    
    DynamicJsonDocument doc(1024);
    JsonObject deviceJson = doc.createNestedObject("device");
    deviceJson["id"] = device->getId();
    deviceJson["name"] = device->getId();
    deviceJson["type"] = device->getType();
    deviceJson["status"] = device->getStatus();
    
    // 如果是智能灯，添加亮度信息
    if (device->getType() == "light") {
        SmartLight* light = static_cast<SmartLight*>(device);
        deviceJson["brightness"] = (int)(light->getBrightness() * 100 / 255);
    }
    
    doc["status"] = "success";
    doc["message"] = "获取设备状态成功";
    
    sendJsonResponse(request, doc);
}

// 处理设备控制API请求
void WebServer::handleControlDevice(AsyncWebServerRequest* request, const JsonVariantConst& json) {
    if (!json.containsKey("id")) {
        request->send(400, "application/json; charset=utf-8",
                      "{\"status\":\"error\",\"message\":\"缺少设备ID\"}");
        return;
    }

    String deviceId = json["id"].as<String>();

    bool hasStatus = json.containsKey("status");
    bool hasBrightness = json.containsKey("brightness");

    bool status = hasStatus ? json["status"].as<bool>() : false;
    int brightness = hasBrightness ? json["brightness"].as<int>() : -1;

    Serial.printf("[CTRL] id=%s status=%s brightness=%d\n",
                  deviceId.c_str(),
                  hasStatus ? (status ? "true" : "false") : "NA",
                  brightness);

    Device* device = deviceManager.getDevice(deviceId);
    if (!device) {
        request->send(404, "application/json; charset=utf-8",
                      "{\"status\":\"error\",\"message\":\"设备不存在\"}");
        return;
    }

    bool result = false;

    if (device->getType() == "light") {
        SmartLight* light = static_cast<SmartLight*>(device);

        if (hasBrightness) {
            int b = brightness;
            if (b < 0) b = 0;
            if (b > 100) b = 100;
            int b255 = b * 255 / 100;
            result = light->setBrightness(b255);
        } else if (hasStatus) {
            result = light->setStatus(status);
        } else {
            request->send(400, "application/json; charset=utf-8",
                          "{\"status\":\"error\",\"message\":\"缺少控制参数\"}");
            return;
        }
    } else if (device->getType() == "switch" || device->getType() == "buzzer") {
        if (!hasStatus) {
            request->send(400, "application/json; charset=utf-8",
                          "{\"status\":\"error\",\"message\":\"缺少status\"}");
            return;
        }
        result = device->setStatus(status);
        if (result && deviceId.startsWith("buzzer_") && !status) {
            alarmManager.muteBuzzerUntilNormal();
        }
    } else {
        request->send(400, "application/json; charset=utf-8",
                      "{\"status\":\"error\",\"message\":\"未知设备类型\"}");
        return;
    }

    if (result) {
        broadcastDeviceUpdate(device);
        DynamicJsonDocument doc(256);
        doc["status"] = "success";
        doc["message"] = "设备控制成功";
        sendJsonResponse(request, doc);
    } else {
        request->send(500, "application/json; charset=utf-8",
                      "{\"status\":\"error\",\"message\":\"设备控制失败\"}");
    }
}

// 处理环境数据API请求
void WebServer::handleGetEnvironmentData(AsyncWebServerRequest* request) {
    DynamicJsonDocument doc(1024);
    JsonObject environment = doc.createNestedObject("environment");
    
    // 获取温度数据
    float temperature = environmentManager.collectSensorData("temperature");
    if (!isnan(temperature)) {
        environment["temperature"] = temperature;
    }
    
    // 获取湿度数据
    float humidity = environmentManager.collectSensorData("humidity");
    if (!isnan(humidity)) {
        environment["humidity"] = humidity;
    }
    
    // 获取光照强度数据
    float lightIntensity = environmentManager.collectSensorData("light");
    if (!isnan(lightIntensity)) {
        environment["lightIntensity"] = lightIntensity;
    }
    
    // 获取烟雾浓度数据
    float smokeDensity = environmentManager.collectSensorData("smoke");
    if (!isnan(smokeDensity)) {
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
    wifi["rssi"] = wiFiManager.getSignalStrength();
    
    doc["status"] = "success";
    doc["message"] = "获取WiFi状态成功";
    
    sendJsonResponse(request, doc);
}

void WebServer::handleGetAlarmSettings(AsyncWebServerRequest* request) {
    DynamicJsonDocument doc(512);
    AlarmThreshold th = alarmManager.getThreshold("smoke");
    JsonObject alarm = doc.createNestedObject("alarm");
    alarm["sensorId"] = "smoke";
    alarm["minThreshold"] = th.minThreshold;
    alarm["maxThreshold"] = th.maxThreshold;
    alarm["enabled"] = th.enabled;
    doc["status"] = "success";
    doc["message"] = "获取告警阈值成功";
    sendJsonResponse(request, doc);
}

void WebServer::handleUpdateAlarmSettings(AsyncWebServerRequest* request, const JsonVariantConst& json) {
    String sensorId = json.containsKey("sensorId") ? json["sensorId"].as<String>() : String("smoke");
    AlarmThreshold th = alarmManager.getThreshold(sensorId);
    bool hasAny = false;
    if (json.containsKey("minThreshold")) {
        th.minThreshold = json["minThreshold"].as<float>();
        hasAny = true;
    }
    if (json.containsKey("maxThreshold")) {
        th.maxThreshold = json["maxThreshold"].as<float>();
        hasAny = true;
    }
    if (json.containsKey("enabled")) {
        th.enabled = json["enabled"].as<bool>();
        hasAny = true;
    }
    th.sensorId = sensorId;
    if (!hasAny) {
        request->send(400, "application/json; charset=utf-8",
                      "{\"status\":\"error\",\"message\":\"缺少阈值参数\"}");
        return;
    }
    bool ok = alarmManager.updateThreshold(th);
    if (ok) {
        if (json.containsKey("enabled") && th.enabled) {
            alarmManager.resetBuzzerMute();
            alarmManager.syncBuzzer(deviceManager);
        } else if (json.containsKey("enabled") && !th.enabled) {
            alarmManager.syncBuzzer(deviceManager);
        }
        DynamicJsonDocument doc(256);
        doc["status"] = "success";
        doc["message"] = "更新告警阈值成功";
        sendJsonResponse(request, doc);
    } else {
        request->send(500, "application/json; charset=utf-8",
                      "{\"status\":\"error\",\"message\":\"更新告警阈值失败\"}");
    }
}

// 发送JSON响应
void WebServer::sendJsonResponse(AsyncWebServerRequest* request, DynamicJsonDocument& doc, int code) {
    String jsonString;
    serializeJson(doc, jsonString);
    
    AsyncWebServerResponse *response = request->beginResponse(code, "application/json", jsonString);
    response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    response->addHeader("Pragma", "no-cache");
    response->addHeader("Expires", "0");
    
    request->send(response);
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
            String msgType = doc["type"].as<String>();
            if (msgType == "ping") {
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

void WebServer::broadcastDeviceUpdate(Device* device) {
    if (!device) return;
    DynamicJsonDocument doc(512);
    doc["type"] = "deviceUpdate";
    JsonObject dev = doc.createNestedObject("device");
    dev["id"] = device->getId();
    dev["name"] = device->getId();
    dev["type"] = device->getType();
    dev["status"] = device->getStatus();
    if (device->getType() == "light") {
        SmartLight* light = static_cast<SmartLight*>(device);
        dev["brightness"] = (int)(light->getBrightness() * 100 / 255);
    }
    String payload;
    serializeJson(doc, payload);
    ws.textAll(payload);
}

// 处理获取配置API请求
void WebServer::handleGetConfig(AsyncWebServerRequest* request) {
    DynamicJsonDocument doc(4096);
    doc["status"] = "success";
    doc["message"] = "获取配置成功";
    
    // 获取完整配置并复制到响应文档
    JsonDocument& config = configManager.getFullConfig();
    doc["config"] = config.as<JsonVariant>();
    
    sendJsonResponse(request, doc);
}

// 处理更新配置API请求
void WebServer::handleUpdateConfig(AsyncWebServerRequest* request, const JsonVariantConst& json) {
    DynamicJsonDocument doc(256);
    
    // 检查是否有配置数据
    if (!json.containsKey("config")) {
        request->send(400, "application/json; charset=utf-8", 
                      "{\"status\":\"error\",\"message\":\"缺少配置数据\"}");
        return;
    }
    
    // 获取配置数据
    const JsonVariantConst& configData = json["config"];
    
    // 更新配置
    JsonDocument& config = configManager.getFullConfig();
    
    // 遍历所有配置项并更新
    for (JsonPairConst kv : configData) {
        const char* key = kv.key().c_str();
        const JsonVariantConst& value = kv.value();
        
        // 特殊处理嵌套配置
        if (value.is<JsonObjectConst>()) {
            // 如果是嵌套对象，递归更新
            JsonObject destObj = config[key].to<JsonObject>();
            for (JsonPairConst innerKv : value.as<JsonObjectConst>()) {
                const char* innerKey = innerKv.key().c_str();
                const JsonVariantConst& innerValue = innerKv.value();
                
                // 处理更深层次的嵌套（如static_ip）
                if (innerValue.is<JsonObjectConst>()) {
                    JsonObject innerDestObj = destObj[innerKey].to<JsonObject>();
                    for (JsonPairConst deepKv : innerValue.as<JsonObjectConst>()) {
                        innerDestObj[deepKv.key()] = deepKv.value();
                    }
                } else {
                    destObj[innerKey] = innerValue;
                }
            }
        } else {
            // 简单值直接更新
            config[key] = value;
        }
    }
    
    // 保存配置到文件
    if (configManager.save()) {
        doc["status"] = "success";
        doc["message"] = "配置保存成功";
        sendJsonResponse(request, doc);
    } else {
        doc["status"] = "error";
        doc["message"] = "配置保存失败";
        sendJsonResponse(request, doc, 500);
    }
}
