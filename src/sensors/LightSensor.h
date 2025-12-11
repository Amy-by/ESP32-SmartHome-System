#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include "EnvironmentSensor.h"

// BH1750传感器相关定义
#define BH1750_ADDR_H 0x5C    // 地址引脚接高电平
#define BH1750_ADDR_L 0x23    // 地址引脚接低电平
#define BH1750_POWER_DOWN 0x00 // 关闭电源
#define BH1750_POWER_ON 0x01   // 打开电源
#define BH1750_RESET 0x07      // 重置数据寄存器
#define BH1750_CONT_H_RES 0x10 // 连续高分辨率模式
#define BH1750_CONT_H_RES2 0x11 // 连续高分辨率模式2
#define BH1750_CONT_L_RES 0x13 // 连续低分辨率模式
#define BH1750_ONE_H_RES 0x20  // 单次高分辨率模式
#define BH1750_ONE_H_RES2 0x21 // 单次高分辨率模式2
#define BH1750_ONE_L_RES 0x23  // 单次低分辨率模式

/**
 * @brief 光线传感器类，继承自EnvironmentSensor抽象类
 * 实现BH1750光线传感器的数据采集功能
 */
class LightSensor : public EnvironmentSensor {
public:
    /**
     * @brief 构造函数，初始化光线传感器对象
     * @param sensorId 传感器ID
     * @param i2cAddr I2C地址（BH1750_ADDR_H 或 BH1750_ADDR_L）
     */
    LightSensor(String sensorId, uint8_t i2cAddr = BH1750_ADDR_L);
    
    /**
     * @brief 析构函数
     */
    ~LightSensor();
    
    /**
     * @brief 初始化传感器
     * @return 成功返回true，失败返回false
     */
    bool initialize() override;
    
    /**
     * @brief 读取传感器数据（光强值）
     * @return 光强值，单位为勒克斯(lux)
     */
    float readData() override;
    
    /**
     * @brief 获取传感器类型
     * @return 传感器类型字符串"light"
     */
    String getType() override;
    
    /**
     * @brief 获取传感器ID
     * @return 传感器ID字符串
     */
    String getId() override;
    
    /**
     * @brief 获取传感器连接的GPIO引脚
     * @return 返回-1，因为BH1750使用I2C接口
     */
    int getPin() override;
    
    /**
     * @brief 获取传感器数据单位
     * @return 数据单位字符串"lux"
     */
    String getUnit() override;
    
    /**
     * @brief 校准传感器
     * @return 成功返回true，失败返回false
     */
    bool calibrate() override;
    
    /**
     * @brief 设置传感器测量模式
     * @param mode 测量模式（参考BH1750相关定义）
     */
    void setMode(uint8_t mode);
    
private:
    /**
     * @brief 向传感器发送命令
     * @param cmd 命令字节
     * @return 成功返回true，失败返回false
     */
    bool sendCommand(uint8_t cmd);
    
    String sensorId;          // 传感器ID
    uint8_t i2cAddr;          // I2C地址
    uint8_t measurementMode;  // 测量模式
    float lastLightIntensity; // 上次读取的光强值
    bool initialized;         // 传感器是否已初始化
};

#endif // LIGHT_SENSOR_H
