#ifndef SMOKE_SENSOR_H
#define SMOKE_SENSOR_H

#include <Arduino.h>
#include "EnvironmentSensor.h"
#include "utils/GPIOController.h"

/**
 * @brief 烟雾传感器类，继承自EnvironmentSensor抽象类
 * 实现MQ-2烟雾传感器的数据采集功能
 */
class SmokeSensor : public EnvironmentSensor {
public:
    /**
     * @brief 构造函数，初始化烟雾传感器对象
     * @param sensorId 传感器ID
     * @param adcPin ADC引脚
     * @param threshold 烟雾浓度阈值
     */
    SmokeSensor(String sensorId, int adcPin, int threshold = 500);
    
    /**
     * @brief 析构函数
     */
    ~SmokeSensor();
    
    /**
     * @brief 初始化传感器
     * @return 成功返回true，失败返回false
     */
    bool initialize() override;
    
    /**
     * @brief 读取传感器数据（烟雾浓度）
     * @return 烟雾浓度值（ADC原始值）
     */
    float readData() override;
    
    /**
     * @brief 获取传感器类型
     * @return 传感器类型字符串"smoke"
     */
    String getType() override;
    
    /**
     * @brief 获取传感器ID
     * @return 传感器ID字符串
     */
    String getId() override;
    
    /**
     * @brief 获取传感器连接的GPIO引脚
     * @return ADC引脚编号
     */
    int getPin() override;
    
    /**
     * @brief 获取传感器数据单位
     * @return 数据单位字符串"ADC"
     */
    String getUnit() override;
    
    /**
     * @brief 校准传感器
     * @return 成功返回true，失败返回false
     */
    bool calibrate() override;
    
    /**
     * @brief 检查是否检测到烟雾
     * @return 检测到烟雾返回true，否则返回false
     */
    bool isSmokeDetected();
    
    /**
     * @brief 设置烟雾浓度阈值
     * @param threshold 阈值ADC值
     */
    void setThreshold(int threshold);
    
    /**
     * @brief 获取当前烟雾浓度阈值
     * @return 阈值ADC值
     */
    int getThreshold();
    
private:
    String sensorId;          // 传感器ID
    int adcPin;               // ADC引脚
    int smokeThreshold;       // 烟雾浓度阈值
    float lastSmokeValue;     // 上次读取的烟雾浓度值
    bool initialized;         // 传感器是否已初始化
    GPIOController gpio;      // GPIO控制器实例
};

#endif // SMOKE_SENSOR_H
