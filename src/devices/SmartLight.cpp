#include "SmartLight.h"

SmartLight::SmartLight(String deviceId, int gpioPin) {
    this->deviceId = deviceId;
    this->gpioPin = gpioPin;
    this->status = false;
    this->brightness = 128; // 默认亮度50%
    
    // 初始化GPIO引脚为输出模式
    gpio.setPinMode(gpioPin, OUTPUT);
    gpio.digitalWrite(gpioPin, LOW); // 初始状态关闭
    
    // 配置PWM
    setPWM(gpioPin, 5000, 8); // 默认5kHz频率，8位分辨率
}

SmartLight::~SmartLight() {
    // 关闭灯光
    gpio.digitalWrite(gpioPin, LOW);
}

bool SmartLight::getStatus() {
    return status;
}

bool SmartLight::setStatus(bool status) {
    this->status = status;
    if (status) {
        gpio.analogWrite(gpioPin, brightness);
    } else {
        // 关闭时必须强制设为0，否则灯可能关不掉
        gpio.analogWrite(gpioPin, 0);
    }
    
    return true;
}

String SmartLight::getType() {
    return "light";
}

String SmartLight::getId() {
    return deviceId;
}

int SmartLight::getPin() {
    return gpioPin;
}

bool SmartLight::setBrightness(int brightness) {
    // 限制亮度值在0-255范围内
    if (brightness < 0) brightness = 0;
    if (brightness > 255) brightness = 255;
    
    this->brightness = brightness;
    
    // 只有当灯光是开启状态时，才更新PWM值
    // 如果是关闭状态，只保存亮度值，不输出PWM，防止“关灯调节亮度”导致灯亮
    if (status) {
        gpio.analogWrite(gpioPin, brightness);
    }
    
    return true;
}

int SmartLight::getBrightness() {
    return brightness;
}

bool SmartLight::setPWM(int pin, int frequency, int resolution) {
    // 配置PWM输出
    return gpio.setPWMPin(pin, frequency, resolution);
}
