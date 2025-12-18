#include "GPIOController.h"

GPIOController::GPIOController() {
    // 构造函数，无需特殊初始化
}

bool GPIOController::setPinMode(int pin, int mode) {
    if (!isPinValid(pin)) {
        return false;
    }
    
    pinMode(pin, mode);
    return true;
}

bool GPIOController::digitalWrite(int pin, bool value) {
    if (!isPinValid(pin)) {
        return false;
    }
    
    ::digitalWrite(pin, value ? HIGH : LOW);
    return true;
}

int GPIOController::digitalRead(int pin) {
    if (!isPinValid(pin)) {
        return LOW;  // 返回默认值
    }
    
    return ::digitalRead(pin);
}

int GPIOController::analogRead(int pin) {
    if (!isPinValid(pin)) {
        return 0;  // 返回默认值
    }
    
    return ::analogRead(pin);
}

bool GPIOController::analogWrite(int pin, int value) {
    if (!isPinValid(pin)) {
        return false;
    }
    
    // 确保值在0-255范围内
    value = constrain(value, 0, 255);
    ::analogWrite(pin, value);
    return true;
}

bool GPIOController::attachInterrupt(int pin, void (*callback)(), int mode) {
    if (!isPinValid(pin)) {
        return false;
    }
    
    attachInterrupt(digitalPinToInterrupt(pin), callback, mode);
    return true;
}

bool GPIOController::detachInterrupt(int pin) {
    if (!isPinValid(pin)) {
        return false;
    }
    
    detachInterrupt(digitalPinToInterrupt(pin));
    return true;
}

bool GPIOController::setPWMFrequency(int pin, int frequency) {
    if (!isPinValid(pin)) {
        return false;
    }
    
    // ESP32的analogWrite频率设置通过ledcSetup完成
    // 这里简化实现，实际项目中可能需要更复杂的处理
    return true;
}

bool GPIOController::setPWMResolution(int pin, int resolution) {
    if (!isPinValid(pin)) {
        return false;
    }
    
    // 确保分辨率在1-16范围内
    if (resolution < 1 || resolution > 16) {
        return false;
    }
    
    // ESP32的PWM分辨率设置通过ledcSetup完成
    // 这里简化实现，实际项目中可能需要更复杂的处理
    return true;
}

bool GPIOController::isPinValid(int pin) {
    return (pin >= MIN_VALID_PIN && pin <= MAX_VALID_PIN);
}

bool GPIOController::setPWMPin(int pin, int frequency, int resolution) {
    if (!isPinValid(pin)) {
        return false;
    }
    if (frequency <= 0) {
        frequency = 5000;
    }
    if (resolution < 1 || resolution > 16) {
        resolution = 8;
    }
    int channel = pin % 16;
    ledcSetup(channel, frequency, resolution);
    ledcAttachPin(pin, channel);
    return true;
}
