#ifndef ENVIRONMENT_SENSOR_H
#define ENVIRONMENT_SENSOR_H

#include <Arduino.h>

/**
 * @brief 环境传感器抽象类，定义了所有环境传感器的基本接口
 * 所有具体环境传感器类都应继承自此类并实现其纯虚函数
 */
class EnvironmentSensor {
public:
    /**
     * @brief 析构函数
     */
    virtual ~EnvironmentSensor() {}
    
    /**
     * @brief 初始化传感器
     * @return 成功返回true，失败返回false
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief 读取传感器数据
     * @return 传感器测量值
     */
    virtual float readData() = 0;
    
    /**
     * @brief 获取传感器类型
     * @return 传感器类型字符串
     */
    virtual String getType() = 0;
    
    /**
     * @brief 获取传感器ID
     * @return 传感器ID字符串
     */
    virtual String getId() = 0;
    
    /**
     * @brief 获取传感器连接的GPIO引脚
     * @return GPIO引脚编号
     */
    virtual int getPin() = 0;
    
    /**
     * @brief 获取传感器数据单位
     * @return 数据单位字符串
     */
    virtual String getUnit() = 0;
    
    /**
     * @brief 校准传感器
     * @return 成功返回true，失败返回false
     */
    virtual bool calibrate() = 0;
};

#endif // ENVIRONMENT_SENSOR_H
