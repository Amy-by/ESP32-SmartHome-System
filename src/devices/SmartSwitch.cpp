#include "SmartSwitch.h"

SmartSwitch::SmartSwitch(String deviceId, int gpioPin) {
    this->deviceId = deviceId;
    this->gpioPin = gpioPin;
    this->status = false;
    
    // 初始化GPIO引脚为输出模式
    gpio.setPinMode(gpioPin, OUTPUT);
    gpio.digitalWrite(gpioPin, LOW); // 初始状态关闭
}

SmartSwitch::~SmartSwitch() {
    // 关闭开关
    gpio.digitalWrite(gpioPin, LOW);
}

bool SmartSwitch::getStatus() {
    return status;
}

bool SmartSwitch::setStatus(bool status) {
    this->status = status;
    gpio.digitalWrite(gpioPin, status ? HIGH : LOW);
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
    return gpio.digitalRead(gpioPin) == HIGH;
}