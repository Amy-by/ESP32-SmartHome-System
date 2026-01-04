#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "SPIFFSStorage.h"
#include "config.h"

class ConfigManager {
private:
    SPIFFSStorage& _spiffsStorage;
    String _configFilePath;
    DynamicJsonDocument _config;
    bool _loaded;

public:
    /**
     * @brief 构造函数
     * @param spiffsStorage SPIFFS存储实例引用
     * @param configFilePath 配置文件路径（默认：/config.json）
     */
    ConfigManager(SPIFFSStorage& spiffsStorage, String configFilePath = "/config.json");
    
    /**
     * @brief 析构函数
     */
    ~ConfigManager();
    
    /**
     * @brief 加载配置文件
     * @return bool 加载是否成功
     */
    bool load();
    
    /**
     * @brief 保存配置文件
     * @return bool 保存是否成功
     */
    bool save();
    
    /**
     * @brief 重置为默认配置
     */
    void resetToDefaults();
    
    /**
     * @brief 获取WiFi SSID
     * @return String WiFi SSID
     */
    String getWiFiSsid();
    
    /**
     * @brief 设置WiFi SSID
     * @param ssid WiFi SSID
     */
    void setWiFiSsid(const String& ssid);
    
    /**
     * @brief 获取WiFi密码
     * @return String WiFi密码
     */
    String getWiFiPassword();
    
    /**
     * @brief 设置WiFi密码
     * @param password WiFi密码
     */
    void setWiFiPassword(const String& password);
    
    /**
     * @brief 获取温度告警阈值
     * @return float 温度告警阈值
     */
    float getTemperatureThreshold();
    
    /**
     * @brief 设置温度告警阈值
     * @param threshold 温度告警阈值
     */
    void setTemperatureThreshold(float threshold);
    
    /**
     * @brief 获取湿度告警阈值
     * @return float 湿度告警阈值
     */
    float getHumidityThreshold();
    
    /**
     * @brief 设置湿度告警阈值
     * @param threshold 湿度告警阈值
     */
    void setHumidityThreshold(float threshold);
    
    /**
     * @brief 获取光照告警阈值
     * @return int 光照告警阈值
     */
    int getLightThreshold();
    
    /**
     * @brief 设置光照告警阈值
     * @param threshold 光照告警阈值
     */
    void setLightThreshold(int threshold);
    
    /**
     * @brief 获取烟雾浓度告警阈值
     * @return int 烟雾浓度告警阈值
     */
    int getSmokeThreshold();
    
    /**
     * @brief 设置烟雾浓度告警阈值
     * @param threshold 烟雾浓度告警阈值
     */
    void setSmokeThreshold(int threshold);
    
    /**
     * @brief 获取最大日志大小
     * @return int 最大日志大小（字节）
     */
    int getMaxLogSize();
    
    /**
     * @brief 设置最大日志大小
     * @param size 最大日志大小（字节）
     */
    void setMaxLogSize(int size);
    
    /**
     * @brief 获取主循环延迟时间
     * @return int 主循环延迟时间（毫秒）
     */
    int getMainLoopDelayMs();
    
    /**
     * @brief 设置主循环延迟时间
     * @param delay 主循环延迟时间（毫秒）
     */
    void setMainLoopDelayMs(int delay);
    
    /**
     * @brief 检查配置是否已加载
     * @return bool 配置是否已加载
     */
    bool isLoaded() const;
    
    /**
     * @brief 获取DHCP启用状态
     * @return bool DHCP是否启用
     */
    bool getWiFiDhcpEnabled();
    
    /**
     * @brief 设置DHCP启用状态
     * @param enabled DHCP是否启用
     */
    void setWiFiDhcpEnabled(bool enabled);
    
    /**
     * @brief 获取静态IP地址
     * @return String 静态IP地址
     */
    String getWiFiStaticIp();
    
    /**
     * @brief 设置静态IP地址
     * @param ip 静态IP地址
     */
    void setWiFiStaticIp(const String& ip);
    
    /**
     * @brief 获取网关地址
     * @return String 网关地址
     */
    String getWiFiGateway();
    
    /**
     * @brief 设置网关地址
     * @param gateway 网关地址
     */
    void setWiFiGateway(const String& gateway);
    
    /**
     * @brief 获取子网掩码
     * @return String 子网掩码
     */
    String getWiFiSubnetMask();
    
    /**
     * @brief 设置子网掩码
     * @param subnetMask 子网掩码
     */
    void setWiFiSubnetMask(const String& subnetMask);
    
    /**
     * @brief 获取DNS服务器地址
     * @return String DNS服务器地址
     */
    String getWiFiDnsServer();
    
    /**
     * @brief 设置DNS服务器地址
     * @param dnsServer DNS服务器地址
     */
    void setWiFiDnsServer(const String& dnsServer);
    
    /**
     * @brief 获取传感器更新间隔
     * @return int 传感器更新间隔（毫秒）
     */
    int getSensorUpdateInterval();
    
    /**
     * @brief 设置传感器更新间隔
     * @param interval 传感器更新间隔（毫秒）
     */
    void setSensorUpdateInterval(int interval);
    
    /**
     * @brief 获取设备轮询间隔
     * @return int 设备轮询间隔（毫秒）
     */
    int getDevicePollingInterval();
    
    /**
     * @brief 设置设备轮询间隔
     * @param interval 设备轮询间隔（毫秒）
     */
    void setDevicePollingInterval(int interval);
    
    /**
     * @brief 获取Web服务器端口
     * @return int Web服务器端口
     */
    int getWebServerPort();
    
    /**
     * @brief 设置Web服务器端口
     * @param port Web服务器端口
     */
    void setWebServerPort(int port);
    
    /**
     * @brief 获取告警持续时间
     * @return int 告警持续时间（秒）
     */
    int getAlarmDuration();
    
    /**
     * @brief 设置告警持续时间
     * @param duration 告警持续时间（秒）
     */
    void setAlarmDuration(int duration);
    
    /**
     * @brief 获取告警启用状态
     * @return bool 告警是否启用
     */
    bool getAlarmEnabled();
    
    /**
     * @brief 设置告警启用状态
     * @param enabled 告警是否启用
     */
    void setAlarmEnabled(bool enabled);
    
    /**
     * @brief 获取调试模式
     * @return bool 调试模式是否启用
     */
    bool getDebugMode();
    
    /**
     * @brief 设置调试模式
     * @param mode 调试模式是否启用
     */
    void setDebugMode(bool mode);
    
    /**
     * @brief 获取数据库服务器地址
     * @return String 数据库服务器地址
     */
    String getDatabaseServer();
    
    /**
     * @brief 设置数据库服务器地址
     * @param server 数据库服务器地址
     */
    void setDatabaseServer(const String& server);
    
    /**
     * @brief 获取数据库端口
     * @return int 数据库端口
     */
    int getDatabasePort();
    
    /**
     * @brief 设置数据库端口
     * @param port 数据库端口
     */
    void setDatabasePort(int port);
    
    /**
     * @brief 获取数据库用户名
     * @return String 数据库用户名
     */
    String getDatabaseUsername();
    
    /**
     * @brief 设置数据库用户名
     * @param username 数据库用户名
     */
    void setDatabaseUsername(const String& username);
    
    /**
     * @brief 获取数据库密码
     * @return String 数据库密码
     */
    String getDatabasePassword();
    
    /**
     * @brief 设置数据库密码
     * @param password 数据库密码
     */
    void setDatabasePassword(const String& password);
    
    /**
     * @brief 获取数据库名称
     * @return String 数据库名称
     */
    String getDatabaseName();
    
    /**
     * @brief 设置数据库名称
     * @param name 数据库名称
     */
    void setDatabaseName(const String& name);
    
    /**
     * @brief 获取完整配置
     * @return JsonDocument& 配置文档
     */
    JsonDocument& getFullConfig();
    
    /**
     * @brief 获取管理员用户名
     * @return String 管理员用户名
     */
    String getAdminUsername();
    
    /**
     * @brief 设置管理员用户名
     * @param username 管理员用户名
     */
    void setAdminUsername(const String& username);
    
    /**
     * @brief 获取管理员密码
     * @return String 管理员密码
     */
    String getAdminPassword();
    
    /**
     * @brief 设置管理员密码
     * @param password 管理员密码
     */
    void setAdminPassword(const String& password);
};

#endif // CONFIGMANAGER_H
