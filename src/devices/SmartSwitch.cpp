#include "SmartSwitch.h"

SmartSwitch::SmartSwitch(String deviceId, int gpioPin, bool activeLow) {
    this->deviceId = deviceId;
    this->gpioPin = gpioPin;
    this->activeLow = activeLow;
    this->status = false;
    
    // 初始化GPIO引脚为输出模式
    gpio.setPinMode(gpioPin, OUTPUT);
    // 初始状态关闭
    // 如果是低电平触发，关闭状态为高电平；否则为低电平
    gpio.digitalWrite(gpioPin, activeLow ? HIGH : LOW); 
}

SmartSwitch::~SmartSwitch() {
    // 关闭开关
    gpio.digitalWrite(gpioPin, activeLow ? HIGH : LOW);
}

bool SmartSwitch::getStatus() {
    return status;
}

bool SmartSwitch::setStatus(bool status) {
    this->status = status;
    int level;
    if (activeLow) {
        // 低电平触发：开启=LOW，关闭=HIGH
        level = status ? LOW : HIGH;
    } else {
        // 高电平触发：开启=HIGH，关闭=LOW
        level = status ? HIGH : LOW;
    }
    gpio.digitalWrite(gpioPin, level);
    return true;
}

String SmartSwitch::getType() {
    return "switch";
}

String SmartSwitch::getId() {
    return deviceId;
}

int SmartSwitch::getPin() {
    return gpioPin;
}

bool SmartSwitch::readState() {
    // 读取GPIO引脚的实际状态
    int val = gpio.digitalRead(gpioPin);
    if (activeLow) {
        return (val == LOW);
    } else {
        return (val == HIGH);
    }
}
