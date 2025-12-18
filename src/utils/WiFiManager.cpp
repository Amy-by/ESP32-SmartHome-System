#include "WiFiManager.h"

WiFiManager::WiFiManager(String ssid, String password) {
    _ssid = ssid;
    _password = password;
}

bool WiFiManager::connect() {
    Serial.print("正在连接WiFi: ");
    Serial.println(_ssid);
    WiFi.disconnect(true);
    delay(500);
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin(_ssid.c_str(), _password.c_str());
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if (millis() - start > 60000) {
            Serial.println("\nWiFi连接仍未成功（60s），放弃本次连接");
            return false;
        }
    }
    Serial.println("\nWiFi连接成功！");
    Serial.print("IP地址: ");
    Serial.println(WiFi.localIP());
    if (_onConnectCallback != nullptr) {
        _onConnectCallback();
    }
    return true;
}

void WiFiManager::disconnect() {
    WiFi.disconnect();
    Serial.println("WiFi已断开连接");
    
    // 调用断开连接回调
    if (_onDisconnectCallback != nullptr) {
        _onDisconnectCallback();
    }
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

IPAddress WiFiManager::getIPAddress() {
    return WiFi.localIP();
}

String WiFiManager::getSSID() {
    return WiFi.SSID();
}

int WiFiManager::getSignalStrength() {
    return WiFi.RSSI();
}

void WiFiManager::setOnConnectCallback(void (*callback)()) {
    _onConnectCallback = callback;
}

void WiFiManager::setOnDisconnectCallback(void (*callback)()) {
    _onDisconnectCallback = callback;
}

bool WiFiManager::checkConnection() {
    if (!isConnected()) {
        Serial.println("WiFi连接已丢失，正在尝试重连...");
        return connect();
    }
    return true;
}

bool WiFiManager::setupAP(String apSSID, String apPassword) {
    Serial.print("正在设置AP模式: ");
    Serial.println(apSSID);
    
    if (apPassword.length() < 8) {
        Serial.println("AP密码长度必须至少8个字符！");
        return false;
    }
    
    WiFi.softAP(apSSID.c_str(), apPassword.c_str());
    
    Serial.print("AP IP地址: ");
    Serial.println(WiFi.softAPIP());
    
    return true;
}

void WiFiManager::update() {
    static unsigned long lastRetry = 0;
    if (WiFi.status() == WL_CONNECTED) {
        return;
    }
    if (millis() - lastRetry > 30000) {
        lastRetry = millis();
        Serial.println("WiFi连接已丢失，尝试重连...");
        connect();
    }
}
