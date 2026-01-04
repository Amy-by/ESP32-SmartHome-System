#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <vector>
#include "ConfigManager.h"
#include "core/Rule.h"

class DatabaseManager {
private:
    WiFiClient _client;
    MySQL_Connection _connection;
    ConfigManager& _configManager;
    String _serverAddress;
    int _serverPort;
    String _username;
    String _password;
    String _databaseName;
    bool _connected;
    unsigned long _lastConnectionAttempt;
    unsigned long _reconnectInterval;

public:
    /**
     * @brief 构造函数
     * @param configManager 配置管理器实例引用
     */
    DatabaseManager(ConfigManager& configManager);
    
    /**
     * @brief 析构函数
     */
    ~DatabaseManager();
    
    /**
     * @brief 初始化数据库连接
     * @return bool 初始化是否成功
     */
    bool init();
    
    /**
     * @brief 连接到数据库
     * @return bool 连接是否成功
     */
    bool connect();
    
    /**
     * @brief 断开数据库连接
     */
    void disconnect();
    
    /**
     * @brief 检查数据库连接状态
     * @return bool 是否已连接
     */
    bool isConnected();
    
    /**
     * @brief 执行SQL查询
     * @param query SQL查询语句
     * @return bool 查询是否成功
     */
    bool executeQuery(const String& query);
    
    /**
     * @brief 执行SQL查询并获取结果
     * @param query SQL查询语句
     * @param result 结果集引用
     * @return bool 查询是否成功
     */
    bool executeSelectQuery(const String& query, String& result);
    
    /**
     * @brief 保存传感器数据到数据库
     * @param sensorId 传感器ID
     * @param type 传感器类型
     * @param value 传感器值
     * @return bool 保存是否成功
     */
    bool saveSensorData(const String& sensorId, const String& type, float value);
    
    /**
     * @brief 更新设备状态
     * @param deviceId 设备ID
     * @param status 设备状态
     * @return bool 更新是否成功
     */
    bool updateDeviceStatus(int deviceId, bool status);
    
    /**
     * @brief 获取设备状态
     * @param deviceId 设备ID
     * @return int 设备状态（-1表示失败）
     */
    int getDeviceStatus(int deviceId);
    
    /**
     * @brief 获取所有设备
     * @param result 结果集引用
     * @return bool 查询是否成功
     */
    bool getAllDevices(String& result);
    
    /**
     * @brief 创建设备
     * @param name 设备名称
     * @param type 设备类型
     * @param pin 设备引脚
     * @param status 设备状态
     * @param brightness 亮度值（仅适用于灯光设备）
     * @return bool 创建是否成功
     */
    bool createDevice(const String& name, const String& type, int pin, bool status = false, int brightness = 0);
    
    /**
     * @brief 删除设备
     * @param deviceId 设备ID
     * @return bool 删除是否成功
     */
    bool deleteDevice(int deviceId);
    
    /**
     * @brief 更新设备亮度
     * @param deviceId 设备ID
     * @param brightness 亮度值
     * @return bool 更新是否成功
     */
    bool updateDeviceBrightness(int deviceId, int brightness);
    
    /**
     * @brief 获取传感器读数
     * @param sensorId 传感器ID
     * @param limit 结果数量限制
     * @param result 结果集引用
     * @return bool 查询是否成功
     */
    bool getSensorReadings(const String& sensorId, int limit, String& result);
    
    /**
     * @brief 获取所有传感器类型
     * @param result 结果集引用
     * @return bool 查询是否成功
     */
    bool getAllSensorTypes(String& result);
    
    /**
     * @brief 获取告警阈值
     * @param sensorId 传感器ID
     * @param result 结果集引用
     * @return bool 查询是否成功
     */
    bool getAlarmThresholds(const String& sensorId, String& result);
    
    /**
     * @brief 更新告警阈值
     * @param sensorId 传感器ID
     * @param type 传感器类型
     * @param minThreshold 最小阈值
     * @param maxThreshold 最大阈值
     * @param enabled 是否启用
     * @return bool 更新是否成功
     */
    bool updateAlarmThreshold(const String& sensorId, const String& type, float minThreshold, float maxThreshold, bool enabled);
    
    /**
     * @brief 获取所有规则
     * @param result 结果集引用
     * @return bool 查询是否成功
     */
    bool getAllRules(String& result);
    
    /**
     * @brief 更新规则状态
     * @param ruleId 规则ID
     * @param enabled 是否启用
     * @return bool 更新是否成功
     */
    bool updateRuleStatus(int ruleId, bool enabled);
    
    /**
     * @brief 创建新规则
     * @param name 规则名称
     * @param enabled 是否启用
     * @param conditions 规则条件
     * @param actions 规则动作
     * @return int 新创建的规则ID，失败返回-1
     */
    int createRule(const String& name, bool enabled, const std::vector<RuleCondition>& conditions, const std::vector<RuleAction>& actions);
    
    /**
     * @brief 更新规则
     * @param ruleId 规则ID
     * @param name 规则名称
     * @param enabled 是否启用
     * @param conditions 规则条件
     * @param actions 规则动作
     * @return bool 更新是否成功
     */
    bool updateRule(int ruleId, const String& name, bool enabled, const std::vector<RuleCondition>& conditions, const std::vector<RuleAction>& actions);
    
    /**
     * @brief 删除规则
     * @param ruleId 规则ID
     * @return bool 删除是否成功
     */
    bool deleteRule(int ruleId);
    
    /**
     * @brief 获取单个规则
     * @param ruleId 规则ID
     * @param result 结果集引用
     * @return bool 查询是否成功
     */
    bool getRuleById(int ruleId, String& result);
    
    /**
     * @brief 删除旧的传感器读数
     * @param days 保留天数
     * @return bool 删除是否成功
     */
    bool deleteOldSensorReadings(int days);
    
    /**
     * @brief 定期维护（重新连接、清理等）
     */
    void maintenance();
    
    /**
     * @brief 设置数据库服务器地址
     * @param address 服务器地址
     */
    void setServerAddress(const String& address);
    
    /**
     * @brief 设置数据库服务器端口
     * @param port 服务器端口
     */
    void setServerPort(int port);
    
    /**
     * @brief 设置数据库用户名
     * @param username 用户名
     */
    void setUsername(const String& username);
    
    /**
     * @brief 设置数据库密码
     * @param password 密码
     */
    void setPassword(const String& password);
    
    /**
     * @brief 设置数据库名称
     * @param databaseName 数据库名称
     */
    void setDatabaseName(const String& databaseName);
    
private:
    /**
     * @brief 转义字符串，防止SQL注入
     * @param str 要转义的字符串
     * @return String 转义后的字符串
     */
    String escapeString(const String& str);
};

#endif // DATABASEMANAGER_H