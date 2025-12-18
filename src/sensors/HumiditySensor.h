#ifndef HUMIDITY_SENSOR_H
#define HUMIDITY_SENSOR_H

#include <Arduino.h>
#include <DHT.h>
#include "EnvironmentSensor.h"

/**
 * @brief 湿度传感器类，继承自EnvironmentSensor抽象类
 * 实现DHT湿度传感器的数据采集功能
 */
class HumiditySensor : public EnvironmentSensor {
public:
    /**
     * @brief 构造函数，初始化湿度传感器对象
     * @param sensorId 传感器ID
     * @param gpioPin GPIO引脚
     * @param sensorType DHT传感器类型（DHT11, DHT12, DHT21, DHT22）
     */
    HumiditySensor(String sensorId, int gpioPin, int sensorType);
    /**
     * @brief 使用共享DHT实例的构造函数
     * @param sensorId 传感器ID
     * @param sharedDht 共享的DHT实例指针
     * @param gpioPin GPIO引脚（用于信息展示）
     * @param sensorType DHT类型（必须与共享实例一致）
     */
    HumiditySensor(String sensorId, DHT* sharedDht, int gpioPin, int sensorType);
    
    /**
     * @brief 析构函数
     */
    ~HumiditySensor();
    
    /**
     * @brief 初始化传感器
     * @return 成功返回true，失败返回false
     */
    bool initialize() override;
    
    /**
     * @brief 读取传感器数据（湿度值）
     * @return 湿度值，单位为百分比
     */
    float readData() override;
    
    /**
     * @brief 获取传感器类型
     * @return 传感器类型字符串"humidity"
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
     * @return 数据单位字符串"%"
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
    float lastHumidity;       // 上次读取的湿度值
    bool initialized;         // 传感器是否已初始化
    bool ownsDHT;             // 是否自行管理DHT实例生命周期
};

#endif // HUMIDITY_SENSOR_H
