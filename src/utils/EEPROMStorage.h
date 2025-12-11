#ifndef EEPROMSTORAGE_H
#define EEPROMSTORAGE_H

#include <Arduino.h>
#include <EEPROM.h>
#include "config.h"
#include "types.h"

// EEPROM存储地址定义
#define EEPROM_START_ADDRESS 0
#define EEPROM_SYSTEM_CONFIG_ADDRESS 0
#define EEPROM_DEVICE_STATES_ADDRESS (EEPROM_SYSTEM_CONFIG_ADDRESS + sizeof(SystemConfig))
#define EEPROM_RULES_ADDRESS (EEPROM_DEVICE_STATES_ADDRESS + sizeof(DeviceState) * 10) // 最多10个设备
#define EEPROM_ALARM_SETTINGS_ADDRESS (EEPROM_RULES_ADDRESS + sizeof(Rule) * 10) // 最多10个规则
#define EEPROM_TOTAL_SIZE (EEPROM_ALARM_SETTINGS_ADDRESS + sizeof(AlarmSetting) * 10) // 最多10个告警设置

class EEPROMStorage {
public:
    /**
     * @brief 构造函数
     */
    EEPROMStorage();
    
    /**
     * @brief 析构函数
     */
    ~EEPROMStorage();
    
    /**
     * @brief 初始化EEPROM
     * @return bool 初始化是否成功
     */
    bool init();
    
    /**
     * @brief 读取系统配置
     * @param config 系统配置结构体引用
     * @return bool 读取是否成功
     */
    bool readSystemConfig(SystemConfig &config);
    
    /**
     * @brief 写入系统配置
     * @param config 系统配置结构体
     * @return bool 写入是否成功
     */
    bool writeSystemConfig(const SystemConfig &config);
    
    /**
     * @brief 读取设备状态
     * @param deviceId 设备ID
     * @param state 设备状态结构体引用
     * @return bool 读取是否成功
     */
    bool readDeviceState(const String &deviceId, DeviceState &state);
    
    /**
     * @brief 写入设备状态
     * @param state 设备状态结构体
     * @return bool 写入是否成功
     */
    bool writeDeviceState(const DeviceState &state);
    
    /**
     * @brief 读取所有设备状态
     * @param states 设备状态数组
     * @param count 设备数量
     * @return bool 读取是否成功
     */
    bool readAllDeviceStates(DeviceState *states, int &count);
    
    /**
     * @brief 读取规则
     * @param ruleId 规则ID
     * @param rule 规则结构体引用
     * @return bool 读取是否成功
     */
    bool readRule(const String &ruleId, Rule &rule);
    
    /**
     * @brief 写入规则
     * @param rule 规则结构体
     * @return bool 写入是否成功
     */
    bool writeRule(const Rule &rule);
    
    /**
     * @brief 读取所有规则
     * @param rules 规则数组
     * @param count 规则数量
     * @return bool 读取是否成功
     */
    bool readAllRules(Rule *rules, int &count);
    
    /**
     * @brief 读取告警设置
     * @param alarmType 告警类型
     * @param setting 告警设置结构体引用
     * @return bool 读取是否成功
     */
    bool readAlarmSetting(const String &alarmType, AlarmSetting &setting);
    
    /**
     * @brief 写入告警设置
     * @param setting 告警设置结构体
     * @return bool 写入是否成功
     */
    bool writeAlarmSetting(const AlarmSetting &setting);
    
    /**
     * @brief 读取所有告警设置
     * @param settings 告警设置数组
     * @param count 告警设置数量
     * @return bool 读取是否成功
     */
    bool readAllAlarmSettings(AlarmSetting *settings, int &count);
    
    /**
     * @brief 清除EEPROM数据
     * @return bool 清除是否成功
     */
    bool clear();
    
private:
    /**
     * @brief 查找设备状态在EEPROM中的索引
     * @param deviceId 设备ID
     * @param index 索引引用
     * @return bool 查找是否成功
     */
    bool findDeviceStateIndex(const String &deviceId, int &index);
    
    /**
     * @brief 查找规则在EEPROM中的索引
     * @param ruleId 规则ID
     * @param index 索引引用
     * @return bool 查找是否成功
     */
    bool findRuleIndex(const String &ruleId, int &index);
    
    /**
     * @brief 查找告警设置在EEPROM中的索引
     * @param alarmType 告警类型
     * @param index 索引引用
     * @return bool 查找是否成功
     */
    bool findAlarmSettingIndex(const String &alarmType, int &index);
    
    /**
     * @brief 读取字符串
     * @param address 存储地址
     * @param str 字符串引用
     * @return int 读取的字节数
     */
    int readString(int address, String &str);
    
    /**
     * @brief 写入字符串
     * @param address 存储地址
     * @param str 字符串
     * @return int 写入的字节数
     */
    int writeString(int address, const String &str);
    
    bool _initialized;
};

#endif // EEPROMSTORAGE_H
