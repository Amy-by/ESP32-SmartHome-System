#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <Arduino.h>
#include <DHT.h>
#include "EnvironmentSensor.h"

/**
 * @brief 温度传感器类，继承自EnvironmentSensor抽象类
 * 实现DHT温度传感器的数据采集功能
 */
class TemperatureSensor : public EnvironmentSensor {
public:
    /**
     * @brief 构造函数，初始化温度传感器对象
     * @param sensorId 传感器ID
     * @param gpioPin GPIO引脚
     * @param sensorType DHT传感器类型（DHT11, DHT12, DHT21, DHT22）
     */
    TemperatureSensor(String sensorId, int gpioPin, int sensorType);
    /**
     * @brief 使用共享DHT实例的构造函数
     * @param sensorId 传感器ID
     * @param sharedDht 共享的DHT实例指针
     * @param gpioPin GPIO引脚（用于信息展示）
     * @param sensorType DHT类型（必须与共享实例一致）
     */
    TemperatureSensor(String sensorId, DHT* sharedDht, int gpioPin, int sensorType);
    
    /**
     * @brief 析构函数
     */
    ~TemperatureSensor();
    
    /**
     * @brief 初始化传感器
     * @return 成功返回true，失败返回false
     */
    bool initialize() override;
    
    /**
     * @brief 读取传感器数据（温度值）
     * @return 温度值，单位为摄氏度
     */
    float readData() override;
    
    /**
     * @brief 获取传感器类型
     * @return 传感器类型字符串"temperature"
     */
    String getType() override;
    
    /**
     * @brief 获取传感器ID
     * @return 传感器ID字符串
     */
    String getId() override;
    
    /**
     * @brief 获取传感器连接的GPIO引脚
     * @return GPIO引脚编号
     */
    int getPin() override;
    
    /**
     * @brief 获取传感器数据单位
     * @return 数据单位字符串"°C"
     */
    String getUnit() override;
    
    /**
     * @brief 校准传感器
     * @return 成功返回true，失败返回false
     */
    bool calibrate() override;
    
private:
    String sensorId;          // 传感器ID
    int gpioPin;              // GPIO引脚
    int sensorType;           // DHT传感器类型
    DHT* dhtSensor;           // DHT传感器实例指针
    float lastTemperature;    // 上次读取的温度值
    bool initialized;         // 传感器是否已初始化
    bool ownsDHT;             // 是否自行管理DHT实例生命周期
};

#endif // TEMPERATURE_SENSOR_H
