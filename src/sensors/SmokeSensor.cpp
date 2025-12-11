#include "SmokeSensor.h"

SmokeSensor::SmokeSensor(String sensorId, int adcPin, int threshold) {
    this->sensorId = sensorId;
    this->adcPin = adcPin;
    this->smokeThreshold = threshold;
    this->lastSmokeValue = 0.0;
    this->initialized = false;
    
    // 初始化GPIO引脚为输入模式
    gpio.setPinMode(adcPin, INPUT);
}

SmokeSensor::~SmokeSensor() {
    // 清理资源
}

bool SmokeSensor::initialize() {
    // MQ-2传感器不需要特殊初始化，只需确保引脚配置正确
    initialized = true;
    return true;
}

float SmokeSensor::readData() {
    if (!initialized) {
        return NAN;
    }
    
    // 读取ADC值
    int adcValue = gpio.analogRead(adcPin);
    
    // 保存上次读取的值
    lastSmokeValue = adcValue;
    return adcValue;
}

String SmokeSensor::getType() {
    return "smoke";
}

String SmokeSensor::getId() {
    return sensorId;
}

int SmokeSensor::getPin() {
    return adcPin;
}

String SmokeSensor::getUnit() {
    return "ADC";
}

bool SmokeSensor::calibrate() {
    // MQ-2传感器校准：读取环境中的基准值
    float baseline = 0.0;
    int sampleCount = 10;
    
    // 读取多个样本并取平均值
    for (int i = 0; i < sampleCount; i++) {
        baseline += readData();
        delay(100);
    }
    baseline /= sampleCount;
    
    // 将阈值设置为基准值的1.5倍
    smokeThreshold = baseline * 1.5;
    return true;
}

bool SmokeSensor::isSmokeDetected() {
    float currentValue = readData();
    return (currentValue > smokeThreshold);
}

void SmokeSensor::setThreshold(int threshold) {
    this->smokeThreshold = threshold;
}

int SmokeSensor::getThreshold() {
    return smokeThreshold;
}
