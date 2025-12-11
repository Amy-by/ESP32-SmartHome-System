#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <Arduino.h>
#include <WiFi.h>

class WiFiManager {
public:
    // 构造函数
    WiFiManager(String ssid, String password);
    
    // 连接到WiFi网络
    bool connect();
    
    // 断开WiFi连接
    void disconnect();
    
    // 检查是否已连接到WiFi
    bool isConnected();
    
    // 获取ESP32的IP地址
    IPAddress getIPAddress();
    
    // 获取当前连接的WiFi名称
    String getSSID();
    
    // 获取WiFi信号强度（RSSI）
    int getSignalStrength();
    
    // 设置WiFi连接成功回调
    void setOnConnectCallback(void (*callback)());
    
    // 设置WiFi断开连接回调
    void setOnDisconnectCallback(void (*callback)());
    
    // 检查WiFi连接状态并自动重连
    bool checkConnection();
    
    // 设置ESP32为AP模式
    bool setupAP(String apSSID, String apPassword);
    
private:
    String _ssid;
    String _password;
    
    // 回调函数指针
    void (*_onConnectCallback)() = nullptr;
    void (*_onDisconnectCallback)() = nullptr;
    
    // 连接超时时间（毫秒）
    static const int CONNECT_TIMEOUT = 10000;
};

#endif // WIFIMANAGER_H
