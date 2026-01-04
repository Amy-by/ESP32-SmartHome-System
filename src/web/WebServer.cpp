// WebServer.cpp - Web服务器类实现文件
// 实现ESP32智能家居控制系统的Web服务器功能

#include "WebServer.h"
#include "../devices/SmartLight.h"
#include "../devices/SmartSwitch.h"
#include "../core/AlarmManager.h"

// 构造函数
WebServer::WebServer(DeviceManager& deviceManager, EnvironmentManager& environmentManager, WiFiManager& wiFiManager, AlarmManager& alarmManager, SPIFFSStorage& spiffsStorage, ConfigManager& configManager, DatabaseManager& databaseManager, RuleEngine& ruleEngine)
    : server(80),
      ws("/ws"),
      deviceManager(deviceManager),
      environmentManager(environmentManager),
      wiFiManager(wiFiManager),
      alarmManager(alarmManager),
      spiffsStorage(spiffsStorage),
      configManager(configManager),
      databaseManager(databaseManager),
      ruleEngine(ruleEngine),
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
    // 管理员登录API
    server.on("/api/admin/login", HTTP_POST, [this](AsyncWebServerRequest* request) {}, 
             nullptr, [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, data, len);
        handleAdminLogin(request, doc.as<JsonVariantConst>());
    });
    
    // 数据库管理API
    // 创建设备
    server.on("/api/admin/devices", HTTP_POST, [this](AsyncWebServerRequest* request) {}, 
             nullptr, [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, data, len);
        handleCreateDevice(request, doc.as<JsonVariantConst>());
    });
    
    // 删除设备
    server.on("/api/admin/devices", HTTP_DELETE, [this](AsyncWebServerRequest* request) {
        handleDeleteDevice(request);
    });
    
    // 编辑设备
    server.on("/api/admin/devices", HTTP_PUT, [this](AsyncWebServerRequest* request) {}, 
             nullptr, [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, data, len);
        handleEditDevice(request, doc.as<JsonVariantConst>());
    });
    
    // 获取传感器数据（带过滤）
    server.on("/api/admin/sensor-data", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetSensorData(request);
    });
    
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
    
    // 规则管理API
    // 创建规则
    server.on("/api/rules", HTTP_POST, [this](AsyncWebServerRequest* request) {}, 
             nullptr, [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, data, len);
        handleCreateRule(request, doc.as<JsonVariantConst>());
    });
    
    // 获取所有规则
    server.on("/api/rules", HTTP_GET, [this](AsyncWebServerRequest* request) {
        handleGetRules(request);
    });
    
    // 更新规则（支持PUT与POST）
    server.on("/api/rules", HTTP_PUT, [this](AsyncWebServerRequest* request) {}, 
             nullptr, [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, data, len);
        handleUpdateRule(request, doc.as<JsonVariantConst>());
    });
    server.on("/api/rules", HTTP_POST, [this](AsyncWebServerRequest* request) {}, 
             nullptr, [this](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, data, len);
        handleUpdateRule(request, doc.as<JsonVariantConst>());
    });
    
    // 删除规则
    server.on("/api/rules", HTTP_DELETE, [this](AsyncWebServerRequest* request) {
        handleDeleteRule(request);
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
    Serial.println("从数据库获取设备列表...");
    DynamicJsonDocument doc(2048);
    JsonArray devices = doc.createNestedArray("devices");
    
    // 从数据库获取所有设备
    String result;
    if (databaseManager.getAllDevices(result)) {
        DynamicJsonDocument dbDoc(1024);
        deserializeJson(dbDoc, result);
        
        // 遍历数据库返回的设备列表
        for (JsonObject deviceJson : dbDoc["rows"].as<JsonArray>()) {
            JsonObject newDevice = devices.createNestedObject();
            newDevice["id"] = deviceJson["id"].as<String>();
            newDevice["name"] = deviceJson["name"].as<String>();
            newDevice["type"] = deviceJson["type"].as<String>();
            newDevice["status"] = deviceJson["status"].as<int>() == 1;
            
            if (newDevice["type"] == "light") {
                newDevice["brightness"] = deviceJson["brightness"].as<int>();
            }
        }
        
        doc["status"] = "success";
        doc["message"] = "从数据库获取设备状态成功";
    } else {
        doc["status"] = "error";
        doc["message"] = "从数据库获取设备状态失败";
    }
    
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
    
    // 从数据库获取设备信息
    String result;
    String query = "SELECT * FROM devices WHERE id = " + deviceId;
    
    if (databaseManager.executeSelectQuery(query, result)) {
        DynamicJsonDocument dbDoc(512);
        deserializeJson(dbDoc, result);
        
        if (dbDoc["rows"].size() > 0) {
            DynamicJsonDocument doc(1024);
            JsonObject deviceJson = doc.createNestedObject("device");
            
            JsonObject dbDevice = dbDoc["rows"][0].as<JsonObject>();
            deviceJson["id"] = dbDevice["id"].as<String>();
            deviceJson["name"] = dbDevice["name"].as<String>();
            deviceJson["type"] = dbDevice["type"].as<String>();
            deviceJson["status"] = dbDevice["status"].as<int>() == 1;
            
            if (deviceJson["type"] == "light") {
                deviceJson["brightness"] = dbDevice["brightness"].as<int>();
            }
            
            doc["status"] = "success";
            doc["message"] = "从数据库获取设备状态成功";
            
            sendJsonResponse(request, doc);
            return;
        }
    }
    
    request->send(404, "text/plain", "设备不存在或数据库查询失败");
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
        // 更新数据库中的设备状态
        int dbDeviceId = deviceId.toInt();
        if (hasStatus) {
            databaseManager.updateDeviceStatus(dbDeviceId, status);
        }
        if (hasBrightness && device->getType() == "light") {
            databaseManager.updateDeviceBrightness(dbDeviceId, brightness);
        }
        
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
    DynamicJsonDocument doc(4096);
    JsonObject environment = doc.createNestedObject("environment");
    
    // 获取实时温度数据
    float temperature = environmentManager.collectSensorData("temperature");
    if (!isnan(temperature)) {
        environment["temperature"] = temperature;
    }
    
    // 获取实时湿度数据
    float humidity = environmentManager.collectSensorData("humidity");
    if (!isnan(humidity)) {
        environment["humidity"] = humidity;
    }
    
    // 获取实时光照强度数据
    float lightIntensity = environmentManager.collectSensorData("light");
    if (!isnan(lightIntensity)) {
        environment["lightIntensity"] = lightIntensity;
    }
    
    // 获取实时烟雾浓度数据
    float smokeDensity = environmentManager.collectSensorData("smoke");
    if (!isnan(smokeDensity)) {
        environment["smokeDensity"] = smokeDensity;
    }
    
    // 添加历史环境数据
    JsonObject historical = doc.createNestedObject("historical");
    
    // 获取温度历史数据
    String tempHistory;
    if (databaseManager.getSensorReadings("DHT22", 10, tempHistory)) {
        DynamicJsonDocument tempDoc(2048);
        deserializeJson(tempDoc, tempHistory);
        historical["temperature"] = tempDoc["rows"];
    }
    
    // 获取湿度历史数据
    String humidityHistory;
    if (databaseManager.getSensorReadings("DHT22", 10, humidityHistory)) {
        DynamicJsonDocument humidityDoc(2048);
        deserializeJson(humidityDoc, humidityHistory);
        historical["humidity"] = humidityDoc["rows"];
    }
    
    // 获取光照历史数据
    String lightHistory;
    if (databaseManager.getSensorReadings("BH1750", 10, lightHistory)) {
        DynamicJsonDocument lightDoc(2048);
        deserializeJson(lightDoc, lightHistory);
        historical["light"] = lightDoc["rows"];
    }
    
    // 获取烟雾历史数据
    String smokeHistory;
    if (databaseManager.getSensorReadings("MQ2", 10, smokeHistory)) {
        DynamicJsonDocument smokeDoc(2048);
        deserializeJson(smokeDoc, smokeHistory);
        historical["smoke"] = smokeDoc["rows"];
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
    DynamicJsonDocument doc(1024);
    JsonArray alarms = doc.createNestedArray("alarms");
    
    // 从数据库获取所有告警阈值
    String result;
    if (databaseManager.getAllSensorTypes(result)) {
        DynamicJsonDocument sensorTypesDoc(512);
        deserializeJson(sensorTypesDoc, result);
        
        for (JsonObject sensorType : sensorTypesDoc["rows"].as<JsonArray>()) {
            String sensorId = sensorType["id"].as<String>();
            String thresholdResult;
            
            if (databaseManager.getAlarmThresholds(sensorId, thresholdResult)) {
                DynamicJsonDocument thresholdDoc(512);
                deserializeJson(thresholdDoc, thresholdResult);
                
                if (thresholdDoc["rows"].size() > 0) {
                    JsonObject thresholdJson = thresholdDoc["rows"][0].as<JsonObject>();
                    JsonObject alarm = alarms.createNestedObject();
                    
                    alarm["sensorId"] = thresholdJson["sensor_id"].as<String>();
                    alarm["minThreshold"] = thresholdJson["min_threshold"].as<float>();
                    alarm["maxThreshold"] = thresholdJson["max_threshold"].as<float>();
                    alarm["enabled"] = thresholdJson["enabled"].as<int>() == 1;
                }
            }
        }
        
        doc["status"] = "success";
        doc["message"] = "从数据库获取告警阈值成功";
    } else {
        doc["status"] = "error";
        doc["message"] = "从数据库获取告警阈值失败";
    }
    
    sendJsonResponse(request, doc);
}

void WebServer::handleUpdateAlarmSettings(AsyncWebServerRequest* request, const JsonVariantConst& json) {
    String sensorId = json.containsKey("sensorId") ? json["sensorId"].as<String>() : String("smoke");
    AlarmThreshold th = alarmManager.getThreshold(sensorId);
    bool hasAny = false;
    float minThreshold = th.minThreshold;
    float maxThreshold = th.maxThreshold;
    bool enabled = th.enabled;
    
    if (json.containsKey("minThreshold")) {
        minThreshold = json["minThreshold"].as<float>();
        th.minThreshold = minThreshold;
        hasAny = true;
    }
    if (json.containsKey("maxThreshold")) {
        maxThreshold = json["maxThreshold"].as<float>();
        th.maxThreshold = maxThreshold;
        hasAny = true;
    }
    if (json.containsKey("enabled")) {
        enabled = json["enabled"].as<bool>();
        th.enabled = enabled;
        hasAny = true;
    }
    th.sensorId = sensorId;
    
    if (!hasAny) {
        request->send(400, "application/json; charset=utf-8",
                      "{\"status\":\"error\",\"message\":\"缺少阈值参数\"}");
        return;
    }
    
    // 更新数据库中的告警阈值
    String sensorType;
    if (sensorId == "DHT22") {
        sensorType = "temperature,humidity";
    } else if (sensorId == "BH1750") {
        sensorType = "light";
    } else if (sensorId == "MQ2") {
        sensorType = "smoke";
    } else {
        sensorType = "unknown";
    }
    
    databaseManager.updateAlarmThreshold(sensorId, sensorType, minThreshold, maxThreshold, enabled);
    
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
    JsonObjectConst configObj = configData.as<JsonObjectConst>();
    for (JsonObjectConst::iterator it = configObj.begin(); it != configObj.end(); ++it) {
        const char* key = it->key().c_str();
        const JsonVariantConst& value = it->value();
        
        // 特殊处理嵌套配置
        if (value.is<JsonObjectConst>()) {
            // 如果是嵌套对象，递归更新
            JsonObject destObj = config[key].to<JsonObject>();
            JsonObjectConst valueObj = value.as<JsonObjectConst>();
            for (JsonObjectConst::iterator innerIt = valueObj.begin(); innerIt != valueObj.end(); ++innerIt) {
                const char* innerKey = innerIt->key().c_str();
                const JsonVariantConst& innerValue = innerIt->value();
                
                // 处理更深层次的嵌套（如static_ip）
                if (innerValue.is<JsonObjectConst>()) {
                    JsonObject innerDestObj = destObj[innerKey].to<JsonObject>();
                    JsonObjectConst innerValueObj = innerValue.as<JsonObjectConst>();
                    for (JsonObjectConst::iterator deepIt = innerValueObj.begin(); deepIt != innerValueObj.end(); ++deepIt) {
                        innerDestObj[deepIt->key()] = deepIt->value();
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

// 处理管理员登录API请求
void WebServer::handleAdminLogin(AsyncWebServerRequest* request, const JsonVariantConst& json) {
    DynamicJsonDocument doc(256);
    
    // 检查是否提供了用户名和密码
    if (!json.containsKey("username") || !json.containsKey("password")) {
        request->send(400, "application/json; charset=utf-8", 
                      "{\"status\":\"error\",\"message\":\"缺少用户名或密码\"}");
        return;
    }
    
    // 获取提供的用户名和密码
    String username = json["username"].as<String>();
    String password = json["password"].as<String>();
    
    // 从配置中获取管理员凭据
    String adminUsername = configManager.getAdminUsername();
    String adminPassword = configManager.getAdminPassword();
    
    // 验证凭据
    if (username == adminUsername && password == adminPassword) {
        doc["status"] = "success";
        doc["message"] = "登录成功";
        sendJsonResponse(request, doc);
    } else {
        doc["status"] = "error";
        doc["message"] = "用户名或密码错误";
        sendJsonResponse(request, doc, 401);
    }
}

// 处理创建设备API请求
void WebServer::handleCreateDevice(AsyncWebServerRequest* request, const JsonVariantConst& json) {
    DynamicJsonDocument doc(256);
    
    // 检查必要参数
    if (!json.containsKey("name") || !json.containsKey("type") || !json.containsKey("pin")) {
        request->send(400, "application/json; charset=utf-8", 
                      "{\"status\":\"error\",\"message\":\"缺少必要参数：name、type或pin\"}");
        return;
    }
    
    String name = json["name"].as<String>();
    String type = json["type"].as<String>();
    int pin = json["pin"].as<int>();
    bool status = json.containsKey("status") ? json["status"].as<bool>() : false;
    int brightness = json.containsKey("brightness") ? json["brightness"].as<int>() : 0;
    
    // 调用数据库管理器创建设备
    if (databaseManager.createDevice(name, type, pin, status, brightness)) {
        doc["status"] = "success";
        doc["message"] = "设备创建成功";
        sendJsonResponse(request, doc);
    } else {
        doc["status"] = "error";
        doc["message"] = "设备创建失败";
        sendJsonResponse(request, doc, 500);
    }
}

// 处理删除设备API请求
void WebServer::handleDeleteDevice(AsyncWebServerRequest* request) {
    DynamicJsonDocument doc(256);
    
    // 检查是否提供了设备ID
    if (!request->hasParam("id")) {
        request->send(400, "application/json; charset=utf-8", 
                      "{\"status\":\"error\",\"message\":\"缺少设备ID\"}");
        return;
    }
    
    String idParam = request->getParam("id")->value();
    int deviceId = idParam.toInt();
    
    // 调用数据库管理器删除设备
    if (databaseManager.deleteDevice(deviceId)) {
        doc["status"] = "success";
        doc["message"] = "设备删除成功";
        sendJsonResponse(request, doc);
    } else {
        doc["status"] = "error";
        doc["message"] = "设备删除失败";
        sendJsonResponse(request, doc, 500);
    }
}

// 处理编辑设备API请求
void WebServer::handleEditDevice(AsyncWebServerRequest* request, const JsonVariantConst& json) {
    DynamicJsonDocument doc(256);
    
    // 检查是否提供了设备ID
    if (!json.containsKey("id")) {
        request->send(400, "application/json; charset=utf-8", 
                      "{\"status\":\"error\",\"message\":\"缺少设备ID\"}");
        return;
    }
    
    int deviceId = json["id"].as<int>();
    
    // 构建更新SQL语句
    String updateQuery = "UPDATE devices SET ";
    bool hasUpdate = false;
    
    if (json.containsKey("name")) {
        if (hasUpdate) updateQuery += ", ";
        updateQuery += "name = '" + json["name"].as<String>() + "'";
        hasUpdate = true;
    }
    
    if (json.containsKey("type")) {
        if (hasUpdate) updateQuery += ", ";
        updateQuery += "type = '" + json["type"].as<String>() + "'";
        hasUpdate = true;
    }
    
    if (json.containsKey("pin")) {
        if (hasUpdate) updateQuery += ", ";
        updateQuery += "pin = " + String(json["pin"].as<int>());
        hasUpdate = true;
    }
    
    if (json.containsKey("status")) {
        if (hasUpdate) updateQuery += ", ";
        updateQuery += "status = " + String(json["status"].as<bool>() ? 1 : 0);
        hasUpdate = true;
    }
    
    if (json.containsKey("brightness")) {
        if (hasUpdate) updateQuery += ", ";
        updateQuery += "brightness = " + String(json["brightness"].as<int>());
        hasUpdate = true;
    }
    
    if (!hasUpdate) {
        request->send(400, "application/json; charset=utf-8", 
                      "{\"status\":\"error\",\"message\":\"没有提供要更新的字段\"}");
        return;
    }
    
    updateQuery += " WHERE id = " + String(deviceId);
    
    // 执行更新操作
    if (databaseManager.executeQuery(updateQuery)) {
        doc["status"] = "success";
        doc["message"] = "设备编辑成功";
        sendJsonResponse(request, doc);
    } else {
        doc["status"] = "error";
        doc["message"] = "设备编辑失败";
        sendJsonResponse(request, doc, 500);
    }
}

// 处理获取传感器数据API请求
void WebServer::handleGetSensorData(AsyncWebServerRequest* request) {
    DynamicJsonDocument doc(4096);
    
    // 获取查询参数
    String sensorId = request->hasParam("sensorId") ? request->getParam("sensorId")->value() : "";
    String sensorType = request->hasParam("type") ? request->getParam("type")->value() : "";
    String limit = request->hasParam("limit") ? request->getParam("limit")->value() : "100";
    String startTime = request->hasParam("startTime") ? request->getParam("startTime")->value() : "";
    String endTime = request->hasParam("endTime") ? request->getParam("endTime")->value() : "";
    
    // 构建查询语句
    String query = "SELECT * FROM sensor_data";
    bool hasWhere = false;
    
    if (!sensorId.isEmpty()) {
        query += hasWhere ? " AND " : " WHERE ";
        query += "sensor_id = '" + sensorId + "'";
        hasWhere = true;
    }
    
    if (!sensorType.isEmpty()) {
        query += hasWhere ? " AND " : " WHERE ";
        query += "type = '" + sensorType + "'";
        hasWhere = true;
    }
    
    if (!startTime.isEmpty() && !endTime.isEmpty()) {
        query += hasWhere ? " AND " : " WHERE ";
        query += "timestamp BETWEEN '" + startTime + "' AND '" + endTime + "'";
        hasWhere = true;
    } else if (!startTime.isEmpty()) {
        query += hasWhere ? " AND " : " WHERE ";
        query += "timestamp >= '" + startTime + "'";
        hasWhere = true;
    } else if (!endTime.isEmpty()) {
        query += hasWhere ? " AND " : " WHERE ";
        query += "timestamp <= '" + endTime + "'";
        hasWhere = true;
    }
    
    // 添加排序和限制
    query += " ORDER BY timestamp DESC LIMIT " + limit;
    
    // 执行查询
    String result;
    if (databaseManager.executeSelectQuery(query, result)) {
        DynamicJsonDocument resultDoc(8192);
        deserializeJson(resultDoc, result);
        doc["status"] = "success";
        doc["message"] = "获取传感器数据成功";
        doc["data"] = resultDoc["rows"];
        sendJsonResponse(request, doc);
    } else {
        doc["status"] = "error";
        doc["message"] = "获取传感器数据失败";
        sendJsonResponse(request, doc, 500);
    }
}

// 处理创建规则API请求
void WebServer::handleCreateRule(AsyncWebServerRequest* request, const JsonVariantConst& json) {
    DynamicJsonDocument doc(1024);
    
    if (json.containsKey("name") && json.containsKey("enabled") && json.containsKey("conditions") && json.containsKey("actions")) {
        String name = json["name"].as<String>();
        bool enabled = json["enabled"].as<bool>();
        
        // 生成唯一规则ID（使用时间戳）
        String id = "rule_" + String(millis());
        
        // 创建新规则
        Rule* rule = new Rule(id, name, enabled);
        
        // 添加条件
        if (json.containsKey("conditions")) {
            JsonArrayConst conditions = json["conditions"].as<JsonArrayConst>();
            for (JsonVariantConst condJson : conditions) {
                if (condJson.containsKey("sensorId") && condJson.containsKey("threshold") && condJson.containsKey("operatorType")) {
                    RuleCondition cond;
                    cond.sensorId = condJson["sensorId"].as<String>();
                    cond.threshold = condJson["threshold"].as<float>();
                    cond.operatorType = condJson["operatorType"].as<String>();
                    rule->addCondition(cond);
                }
            }
        }
        
        // 添加动作
        if (json.containsKey("actions")) {
            JsonArrayConst actions = json["actions"].as<JsonArrayConst>();
            for (JsonVariantConst actJson : actions) {
                if (actJson.containsKey("deviceId") && actJson.containsKey("targetStatus")) {
                    RuleAction act;
                    act.deviceId = actJson["deviceId"].as<String>();
                    act.targetStatus = actJson["targetStatus"].as<bool>();
                    rule->addAction(act);
                }
            }
        }
        
        // 添加规则到规则引擎
        ruleEngine.addRule(rule);
        
        // 保存规则到SPIFFS
        if (ruleEngine.saveRules()) {
            doc["status"] = "success";
            doc["message"] = "规则创建成功";
            doc["data"]["id"] = rule->getId();
            sendJsonResponse(request, doc);
        } else {
            doc["status"] = "error";
            doc["message"] = "规则保存失败";
            sendJsonResponse(request, doc, 500);
        }
    } else {
        doc["status"] = "error";
        doc["message"] = "参数不完整";
        sendJsonResponse(request, doc, 400);
    }
}

// 处理获取所有规则API请求
void WebServer::handleGetRules(AsyncWebServerRequest* request) {
    DynamicJsonDocument doc(4096);
    JsonArray rulesArray = doc.createNestedArray("rules");
    
    // 获取所有规则
    std::vector<Rule*> rules = ruleEngine.getAllRules();
    
    for (Rule* rule : rules) {
        JsonObject ruleJson = rulesArray.createNestedObject();
        ruleJson["id"] = rule->getId();
        ruleJson["name"] = rule->getName();
        ruleJson["enabled"] = rule->isEnabled();
        
        // 添加条件
        JsonArray conditionsArray = ruleJson.createNestedArray("conditions");
        for (const RuleCondition& cond : rule->getConditions()) {
            JsonObject condJson = conditionsArray.createNestedObject();
            condJson["sensorId"] = cond.sensorId;
            condJson["threshold"] = cond.threshold;
            condJson["operatorType"] = cond.operatorType;
        }
        
        // 添加动作
        JsonArray actionsArray = ruleJson.createNestedArray("actions");
        for (const RuleAction& act : rule->getActions()) {
            JsonObject actJson = actionsArray.createNestedObject();
            actJson["deviceId"] = act.deviceId;
            actJson["targetStatus"] = act.targetStatus;
        }
    }
    
    doc["status"] = "success";
    doc["message"] = "获取规则列表成功";
    sendJsonResponse(request, doc);
}

// 处理更新规则API请求
void WebServer::handleUpdateRule(AsyncWebServerRequest* request, const JsonVariantConst& json) {
    DynamicJsonDocument doc(1024);
    
    if (json.containsKey("id") && json.containsKey("name") && json.containsKey("enabled") && json.containsKey("conditions") && json.containsKey("actions")) {
        String id = json["id"].as<String>();
        String name = json["name"].as<String>();
        bool enabled = json["enabled"].as<bool>();
        
        // 获取现有规则
        Rule* rule = ruleEngine.getRule(id);
        
        if (rule) {
            // 更新规则信息
            rule->setName(name);
            rule->setEnabled(enabled);
            
            // 清空现有条件和动作
            rule->clearConditions();
            rule->clearActions();
            
            // 添加新条件
            if (json.containsKey("conditions")) {
                JsonArrayConst conditions = json["conditions"].as<JsonArrayConst>();
                for (JsonVariantConst condJson : conditions) {
                    if (condJson.containsKey("sensorId") && condJson.containsKey("threshold") && condJson.containsKey("operatorType")) {
                        RuleCondition cond;
                        cond.sensorId = condJson["sensorId"].as<String>();
                        cond.threshold = condJson["threshold"].as<float>();
                        cond.operatorType = condJson["operatorType"].as<String>();
                        rule->addCondition(cond);
                    }
                }
            }
            
            // 添加新动作
            if (json.containsKey("actions")) {
                JsonArrayConst actions = json["actions"].as<JsonArrayConst>();
                for (JsonVariantConst actJson : actions) {
                    if (actJson.containsKey("deviceId") && actJson.containsKey("targetStatus")) {
                        RuleAction act;
                        act.deviceId = actJson["deviceId"].as<String>();
                        act.targetStatus = actJson["targetStatus"].as<bool>();
                        rule->addAction(act);
                    }
                }
            }
            
            // 保存规则到SPIFFS
            if (ruleEngine.saveRules()) {
                doc["status"] = "success";
                doc["message"] = "规则更新成功";
                sendJsonResponse(request, doc);
            } else {
                doc["status"] = "error";
                doc["message"] = "规则保存失败";
                sendJsonResponse(request, doc, 500);
            }
        } else {
            doc["status"] = "error";
            doc["message"] = "未找到指定规则";
            sendJsonResponse(request, doc, 404);
        }
    } else {
        doc["status"] = "error";
        doc["message"] = "参数不完整";
        sendJsonResponse(request, doc, 400);
    }
}

// 处理删除规则API请求
void WebServer::handleDeleteRule(AsyncWebServerRequest* request) {
    DynamicJsonDocument doc(1024);
    
    if (request->hasParam("id", true)) {
        String id = request->getParam("id", true)->value();
        
        // 删除规则
        if (ruleEngine.removeRule(id)) {
            // 保存规则到SPIFFS
            if (ruleEngine.saveRules()) {
                doc["status"] = "success";
                doc["message"] = "规则删除成功";
                sendJsonResponse(request, doc);
            } else {
                doc["status"] = "error";
                doc["message"] = "规则保存失败";
                sendJsonResponse(request, doc, 500);
            }
        } else {
            doc["status"] = "error";
            doc["message"] = "未找到指定规则";
            sendJsonResponse(request, doc, 404);
        }
    } else {
        doc["status"] = "error";
        doc["message"] = "缺少参数id";
        sendJsonResponse(request, doc, 400);
    }
}
