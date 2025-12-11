#include "LightSensor.h"

LightSensor::LightSensor(String sensorId, uint8_t i2cAddr) {
    this->sensorId = sensorId;
    this->i2cAddr = i2cAddr;
    this->measurementMode = BH1750_CONT_H_RES; // 默认使用连续高分辨率模式
    this->lastLightIntensity = 0.0;
    this->initialized = false;
}

LightSensor::~LightSensor() {
    // 关闭传感器电源
    if (initialized) {
        sendCommand(BH1750_POWER_DOWN);
    }
}

bool LightSensor::initialize() {
    // 初始化Wire库
    Wire.begin();
    
    // 打开传感器电源
    if (!sendCommand(BH1750_POWER_ON)) {
        return false;
    }
    
    // 重置数据寄存器
    if (!sendCommand(BH1750_RESET)) {
        return false;
    }
    
    // 设置测量模式
    if (!sendCommand(measurementMode)) {
        return false;
    }
    
    initialized = true;
    return true;
}

float LightSensor::readData() {
    if (!initialized) {
        return NAN;
    }
    
    // 等待测量完成（根据模式不同，等待时间不同）
    switch (measurementMode) {
        case BH1750_CONT_L_RES:
        case BH1750_ONE_L_RES:
            delay(24);
            break;
        case BH1750_CONT_H_RES:
        case BH1750_CONT_H_RES2:
        case BH1750_ONE_H_RES:
        case BH1750_ONE_H_RES2:
            delay(180);
            break;
    }
    
    // 请求读取2字节数据
    Wire.requestFrom(i2cAddr, 2);
    
    if (Wire.available() != 2) {
        return NAN;
    }
    
    // 读取数据
    uint16_t rawData = (Wire.read() << 8) | Wire.read();
    
    // 计算光强值（单位：勒克斯）
    float lightIntensity = rawData / 1.2;
    
    // 保存上次读取的值
    lastLightIntensity = lightIntensity;
    return lightIntensity;
}

String LightSensor::getType() {
    return "light";
}

String LightSensor::getId() {
    return sensorId;
}

int LightSensor::getPin() {
    return -1; // BH1750使用I2C接口，没有单独的GPIO引脚
}

String LightSensor::getUnit() {
    return "lux";
}

bool LightSensor::calibrate() {
    // TODO: 实现光线传感器校准功能
    // 暂时返回true表示成功
    return true;
}

void LightSensor::setMode(uint8_t mode) {
    if (initialized) {
        sendCommand(mode);
    }
    measurementMode = mode;
}

bool LightSensor::sendCommand(uint8_t cmd) {
    Wire.beginTransmission(i2cAddr);
    Wire.write(cmd);
    int result = Wire.endTransmission();
    return (result == 0);
}
