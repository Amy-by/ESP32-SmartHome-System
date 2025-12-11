#ifndef ENVIRONMENT_MANAGER_H
#define ENVIRONMENT_MANAGER_H

#include <Arduino.h>
#include <vector>
#include "EnvironmentSensor.h"

/**
 * @brief 环境传感器管理器类，负责管理所有环境传感器
 * 提供传感器的添加、移除、数据采集和管理等功能
 */
class EnvironmentManager {
public:
    /**
     * @brief 构造函数，初始化环境传感器管理器
     */
    EnvironmentManager();
    
    /**
     * @brief 析构函数
     */
    ~EnvironmentManager();
    
    /**
     * @brief 添加传感器到管理器
     * @param sensor 传感器指针
     * @return 成功返回true，失败返回false
     */
    bool addSensor(EnvironmentSensor* sensor);
    
    /**
     * @brief 从管理器中移除传感器
     * @param sensorId 传感器ID
     * @return 成功返回true，失败返回false
     */
    bool removeSensor(String sensorId);
    
    /**
     * @brief 获取指定ID的传感器指针
     * @param sensorId 传感器ID
     * @return 传感器指针，若未找到返回nullptr
     */
    EnvironmentSensor* getSensor(String sensorId);
    
    /**
     * @brief 获取所有传感器列表
     * @return 包含所有传感器指针的vector
     */
    std::vector<EnvironmentSensor*> getAllSensors();
    
    /**
     * @brief 采集所有传感器数据
     * @return 成功返回true，失败返回false
     */
    bool collectAllSensorData();
    
    /**
     * @brief 采集指定传感器数据
     * @param sensorId 传感器ID
     * @return 传感器测量值，若传感器不存在返回NAN
     */
    float collectSensorData(String sensorId);
    
    /**
     * @brief 初始化所有传感器
     * @return 成功返回true，失败返回false
     */
    bool initializeAllSensors();
    
    /**
     * @brief 获取传感器数量
     * @return 传感器数量
     */
    int getSensorCount();
    
    /**
     * @brief 保存所有传感器配置到存储
     * @return 成功返回true，失败返回false
     */
    bool saveSensorConfigurations();
    
    /**
     * @brief 从存储加载传感器配置
     * @return 成功返回true，失败返回false
     */
    bool loadSensorConfigurations();
    
private:
    // 传感器列表
    std::vector<EnvironmentSensor*> sensors;
};

#endif // ENVIRONMENT_MANAGER_H
