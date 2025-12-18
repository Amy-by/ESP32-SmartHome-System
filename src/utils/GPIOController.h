#ifndef GPIOCONTROLLER_H
#define GPIOCONTROLLER_H

#include <Arduino.h>

class GPIOController {
public:
    // 构造函数
    GPIOController();
    
    // 设置GPIO引脚模式
    bool setPinMode(int pin, int mode);
    
    // 设置GPIO引脚输出值
    bool digitalWrite(int pin, bool value);
    
    // 读取GPIO引脚输入值
    int digitalRead(int pin);
    
    // 读取GPIO引脚模拟值（0-4095）
    int analogRead(int pin);
    
    // 设置GPIO引脚PWM输出值
    bool analogWrite(int pin, int value);
    
    // 配置GPIO引脚的PWM参数（频率与分辨率）
    bool setPWMPin(int pin, int frequency, int resolution);
    
    // 为GPIO引脚添加中断
    bool attachInterrupt(int pin, void (*callback)(), int mode);
    
    // 移除GPIO引脚的中断
    bool detachInterrupt(int pin);
    
    // 设置GPIO引脚PWM频率
    bool setPWMFrequency(int pin, int frequency);
    
    // 设置GPIO引脚PWM分辨率
    bool setPWMResolution(int pin, int resolution);
    
    // 检查引脚是否有效
    bool isPinValid(int pin);
    
private:
    // ESP32的有效GPIO引脚范围（根据具体开发板可能需要调整）
    static const int MIN_VALID_PIN = 0;
    static const int MAX_VALID_PIN = 39;
};

#endif // GPIOCONTROLLER_H
