#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>

/**
 * @brief 设备抽象类，定义了所有设备的基本接口
 * 所有具体设备类都应继承自此类并实现其纯虚函数
 */
class Device {
public:
    /**
     * @brief 析构函数
     */
    virtual ~Device() {}
    
    /**
     * @brief 获取设备当前状态
     * @return 设备状态，true表示开启，false表示关闭
     */
    virtual bool getStatus() = 0;
    
    /**
     * @brief 设置设备状态
     * @param status 设备状态，true表示开启，false表示关闭
     * @return 成功返回true，失败返回false
     */
    virtual bool setStatus(bool status) = 0;
    
    /**
     * @brief 获取设备类型
     * @return 设备类型字符串
     */
    virtual String getType() = 0;
    
    /**
     * @brief 获取设备ID
     * @return 设备ID字符串
     */
    virtual String getId() = 0;
    
    /**
     * @brief 获取设备连接的GPIO引脚
     * @return GPIO引脚编号
     */
    virtual int getPin() = 0;
};

#endif // DEVICE_H