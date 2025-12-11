#ifndef SMARTLIGHT_H
#define SMARTLIGHT_H

#include <Arduino.h>
#include "Device.h"
#include "utils/GPIOController.h"

/**
 * @brief 智能灯类，继承自Device抽象类
 * 实现智能灯的状态控制和亮度调节功能
 */
class SmartLight : public Device {
public:
    /**
     * @brief 构造函数，初始化智能灯对象
     * @param deviceId 设备ID
     * @param gpioPin GPIO引脚
     */
    SmartLight(String deviceId, int gpioPin);
    
    /**
     * @brief 析构函数
     */
    ~SmartLight();
    
    /**
     * @brief 获取智能灯当前状态
     * @return 智能灯状态，true表示开启，false表示关闭
     */
    bool getStatus() override;
    
    /**
     * @brief 设置智能灯状态
     * @param status 智能灯状态
     * @return 成功返回true，失败返回false
     */
    bool setStatus(bool status) override;
    
    /**
     * @brief 获取设备类型
     * @return 设备类型字符串"light"
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
     * @brief 设置智能灯亮度
     * @param brightness 亮度值（0-255）
     * @return 成功返回true，失败返回false
     */
    bool setBrightness(int brightness);
    
    /**
     * @brief 获取当前亮度值
     * @return 亮度值（0-255）
     */
    int getBrightness();
    
    /**
     * @brief 配置PWM输出
     * @param pin PWM引脚
     * @param frequency PWM频率
     * @param resolution PWM分辨率
     * @return 成功返回true，失败返回false
     */
    bool setPWM(int pin, int frequency, int resolution);
    
private:
    String deviceId;          // 设备ID
    int gpioPin;              // GPIO引脚
    bool status;              // 设备状态
    int brightness;           // 亮度值（0-255）
    GPIOController gpio;      // GPIO控制器实例
};

#endif // SMARTLIGHT_H