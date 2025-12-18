#include "HumiditySensor.h"

HumiditySensor::HumiditySensor(String sensorId, int gpioPin, int sensorType) {
    this->sensorId = sensorId;
    this->gpioPin = gpioPin;
    this->sensorType = sensorType;
    this->lastHumidity = 0.0;
    this->initialized = false;
    
    // 创建DHT传感器实例
    dhtSensor = new DHT(gpioPin, sensorType);
    ownsDHT = true;
}

HumiditySensor::~HumiditySensor() {
    // 释放DHT传感器实例
    if (ownsDHT && dhtSensor != nullptr) {
        delete dhtSensor;
    }
    dhtSensor = nullptr;
}

bool HumiditySensor::initialize() {
    if (dhtSensor == nullptr) {
        return false;
    }
    
    // 初始化DHT传感器
    if (ownsDHT) {
        dhtSensor->begin();
    }
    initialized = true;
    return true;
}

float HumiditySensor::readData() {
    if (!initialized || dhtSensor == nullptr) {
        return NAN;
    }
    
    // 读取湿度值
    float humidity = dhtSensor->readHumidity();
    
    // 检查是否读取成功
    if (isnan(humidity)) {
        return NAN;
    }
    
    // 保存上次读取的湿度值
    lastHumidity = humidity;
    return humidity;
}

HumiditySensor::HumiditySensor(String sensorId, DHT* sharedDht, int gpioPin, int sensorType) {
    this->sensorId = sensorId;
    this->gpioPin = gpioPin;
    this->sensorType = sensorType;
    this->lastHumidity = 0.0;
    this->initialized = false;
    this->dhtSensor = sharedDht;
    this->ownsDHT = false;
}
String HumiditySensor::getType() {
    return "humidity";
}

String HumiditySensor::getId() {
    return sensorId;
}

int HumiditySensor::getPin() {
    return gpioPin;
}

String HumiditySensor::getUnit() {
    return "%";
}

bool HumiditySensor::calibrate() {
    // TODO: 实现湿度传感器校准功能
    // 暂时返回true表示成功
    return true;
}
