#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor(String sensorId, int gpioPin, int sensorType) {
    this->sensorId = sensorId;
    this->gpioPin = gpioPin;
    this->sensorType = sensorType;
    this->lastTemperature = 0.0;
    this->initialized = false;
    
    // 创建DHT传感器实例
    dhtSensor = new DHT(gpioPin, sensorType);
}

TemperatureSensor::~TemperatureSensor() {
    // 释放DHT传感器实例
    delete dhtSensor;
    dhtSensor = nullptr;
}

bool TemperatureSensor::initialize() {
    if (dhtSensor == nullptr) {
        return false;
    }
    
    // 初始化DHT传感器
    dhtSensor->begin();
    initialized = true;
    return true;
}

float TemperatureSensor::readData() {
    if (!initialized || dhtSensor == nullptr) {
        return NAN;
    }
    
    // 读取温度值
    float temperature = dhtSensor->readTemperature();
    
    // 检查是否读取成功
    if (isnan(temperature)) {
        return NAN;
    }
    
    // 保存上次读取的温度值
    lastTemperature = temperature;
    return temperature;
}

String TemperatureSensor::getType() {
    return "temperature";
}

String TemperatureSensor::getId() {
    return sensorId;
}

int TemperatureSensor::getPin() {
    return gpioPin;
}

String TemperatureSensor::getUnit() {
    return "°C";
}

bool TemperatureSensor::calibrate() {
    // TODO: 实现温度传感器校准功能
    // 暂时返回true表示成功
    return true;
}
