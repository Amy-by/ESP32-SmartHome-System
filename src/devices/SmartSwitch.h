#ifndef SMART_SWITCH_H
#define SMART_SWITCH_H

#include <Arduino.h>
#include "Device.h"
#include "utils/GPIOController.h"

/**
 * @brief 智能开关类，继承自Device抽象类
 * 实现智能开关的状态控制和读取功能
 */
class SmartSwitch : public Device {
public:
    /**
     * @brief 构造函数，初始化智能开关对象
     * @param deviceId 设备ID
     * @param gpioPin GPIO引脚
     */
    SmartSwitch(String deviceId, int gpioPin);
    
    /**
     * @brief 析构函数
     */
    ~SmartSwitch();
    
    /**
     * @brief 获取智能开关当前状态
     * @return 智能开关状态，true表示开启，false表示关闭
     */
    bool getStatus() override;
    
    /**
     * @brief 设置智能开关状态
     * @param status 智能开关状态
     * @return 成功返回true，失败返回false
     */
    bool setStatus(bool status) override;
    
    /**
     * @brief 获取设备类型
     * @return 设备类型字符串"switch"
     */
    String getType() override;
    
    /**
     * @brief 获取设备ID
     * @return 设备ID字符串
     */
    String getId() override;
    
    /**
     * @brief 获取设备连接的GPIO引脚
     * @return GPIO引脚编号
     */
    int getPin() override;
    
    /**
     * @brief 读取GPIO引脚当前状态
     * @return GPIO引脚状态
     */
    bool readState();
    
private:
    String deviceId;          // 设备ID
    int gpioPin;              // GPIO引脚
    bool status;              // 设备状态
    GPIOController gpio;      // GPIO控制器实例
};

#endif // SMART_SWITCH_H