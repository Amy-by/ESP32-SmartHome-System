#ifndef ALARMMANAGER_H
#define ALARMMANAGER_H

#include <Arduino.h>
#include <vector>
#include "sensors/EnvironmentManager.h"
#include "devices/DeviceManager.h"
#include "utils/DataLogger.h"
#include "utils/SPIFFSStorage.h"
#include "definitions.h"

/**
 * @brief 告警阈值结构体，定义传感器的告警阈值
 */
struct AlarmThreshold {
    String sensorId;      // 传感器ID
    float minThreshold;   // 最小阈值，-1表示无下限
    float maxThreshold;   // 最大阈值，-1表示无上限
    bool enabled;         // 阈值是否启用
};

/**
 * @brief 告警状态结构体，定义当前告警状态
 */
struct AlarmStatus {
    String sensorId;      // 传感器ID
    bool isActive;        // 告警是否激活
    float currentValue;   // 当前传感器值
    String message;       // 告警消息
    unsigned long time;   // 告警触发时间
};

/**
 * @brief 告警联动动作结构体，定义告警触发后执行的动作
 */
struct AlarmAction {
    String deviceId;      // 设备ID
    bool targetStatus;    // 目标状态：true表示开启，false表示关闭
};

/**
 * @brief 告警管理器类，负责阈值管理、告警触发和联动控制
 * 提供阈值的添加、移除、启用、禁用等功能，并能够监测传感器数据和触发告警
 */
class AlarmManager {
public:
    /**
     * @brief 构造函数
     * @param dataLogger 数据记录器指针
     */
    AlarmManager(DataLogger* dataLogger, SPIFFSStorage* storage = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~AlarmManager();
    
    /**
     * @brief 添加告警阈值
     * @param threshold 告警阈值
     * @return 成功返回true，失败返回false
     */
    bool addThreshold(const AlarmThreshold& threshold);
    
    /**
     * @brief 更新告警阈值
     * @param threshold 告警阈值
     * @return 成功返回true，失败返回false
     */
    bool updateThreshold(const AlarmThreshold& threshold);
    
    /**
     * @brief 移除告警阈值
     * @param sensorId 传感器ID
     * @return 成功返回true，失败返回false
     */
    bool removeThreshold(String sensorId);
    
    /**
     * @brief 获取指定传感器的告警阈值
     * @param sensorId 传感器ID
     * @return 告警阈值结构体，若未找到返回默认值
     */
    AlarmThreshold getThreshold(String sensorId);
    
    /**
     * @brief 添加告警联动动作
     * @param action 告警联动动作
     */
    void addAlarmAction(const AlarmAction& action);
    
    /**
     * @brief 检查传感器数据是否触发告警
     * @param envManager 环境传感器管理器
     * @return 触发的告警状态列表
     */
    std::vector<AlarmStatus> checkAlarms(EnvironmentManager& envManager);
    
    /**
     * @brief 执行告警联动动作
     * @param deviceManager 设备管理器
     * @return 动作执行成功返回true，否则返回false
     */
    bool executeAlarmActions(DeviceManager& deviceManager);
    
    /**
     * @brief 获取当前激活的告警
     * @return 激活的告警状态列表
     */
    std::vector<AlarmStatus> getActiveAlarms();
    
    /**
     * @brief 清除指定传感器的告警
     * @param sensorId 传感器ID
     * @return 成功返回true，失败返回false
     */
    bool clearAlarm(String sensorId);
    
    /**
     * @brief 清除所有告警
     * @return 成功返回true，失败返回false
     */
    bool clearAllAlarms();
    
    void setBuzzer(String id);
    void muteBuzzerUntilNormal();
    void resetBuzzerMute();
    bool syncBuzzer(DeviceManager& deviceManager);
    
private:
    std::vector<AlarmThreshold> thresholds;    // 告警阈值列表
    std::vector<AlarmStatus> activeAlarms;     // 当前激活的告警列表
    std::vector<AlarmAction> alarmActions;     // 告警联动动作列表
    DataLogger* dataLogger;                    // 数据记录器指针
    SPIFFSStorage* storage;                    // 存储指针（用于持久化配置）
    String buzzerId;
    bool buzzerMuted = false;
    bool lastBuzzerState = false;
    
    /**
     * @brief 触发告警
     * @param sensorId 传感器ID
     * @param currentValue 当前传感器值
     * @param message 告警消息
     */
    void triggerAlarm(String sensorId, float currentValue, String message);
    
public:
    /**
     * @brief 从存储加载阈值配置
     * @return 成功返回true，失败返回false
     */
    bool loadFromStorage();
    
    /**
     * @brief 保存当前阈值配置到存储
     * @return 成功返回true，失败返回false
     */
    bool saveToStorage();
};

#endif // ALARMMANAGER_H
