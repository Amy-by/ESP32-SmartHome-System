#include "LightSensor.h"

LightSensor::LightSensor(String sensorId, uint8_t i2cAddr) {
    this->sensorId = sensorId;
    this->i2cAddr = i2cAddr;
    this->measurementMode = BH1750_CONT_H_RES;
    this->lastLightIntensity = 0.0;
    this->initialized = false;
    this->useAnalog = false;
    this->analogPin = -1;
}

LightSensor::~LightSensor() {
    if (initialized) {
        sendCommand(BH1750_POWER_DOWN);
    }
}

bool LightSensor::initialize() {
    if (useAnalog) {
        pinMode(analogPin, INPUT);
        initialized = true;
        return true;
    } else {
        Wire.begin();
        if (!sendCommand(BH1750_POWER_ON)) return false;
        if (!sendCommand(BH1750_RESET)) return false;
        if (!sendCommand(measurementMode)) return false;
        initialized = true;
        return true;
    }
}

float LightSensor::readData() {
    if (!initialized) {
        return NAN;
    }
    if (useAnalog) {
        int raw = analogRead(analogPin);
        lastLightIntensity = raw;
        return (float)raw;
    } else {
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
        Wire.requestFrom(i2cAddr, (uint8_t)2);
        if (Wire.available() != 2) {
            return NAN;
        }
        uint16_t rawData = (Wire.read() << 8) | Wire.read();
        float lightIntensity = rawData / 1.2;
        lastLightIntensity = lightIntensity;
        return lightIntensity;
    }
}

String LightSensor::getType() {
    return "light";
}

String LightSensor::getId() {
    return sensorId;
}

String LightSensor::getUnit() {
    return "lux";
}

bool LightSensor::calibrate() {
    return true;
}

void LightSensor::setMode(uint8_t mode) {
    if (initialized) {
        sendCommand(mode);
    }
    measurementMode = mode;
}

int LightSensor::getPin() {
    if (useAnalog) return analogPin;
    return -1;
}

bool LightSensor::sendCommand(uint8_t cmd) {
    Wire.beginTransmission(i2cAddr);
    Wire.write(cmd);
    int result = Wire.endTransmission();
    return (result == 0);
}

LightSensor::LightSensor(String sensorId, int analogPin) {
    this->sensorId = sensorId;
    this->analogPin = analogPin;
    this->useAnalog = true;
    this->i2cAddr = BH1750_ADDR_L;
    this->measurementMode = BH1750_CONT_H_RES;
    this->lastLightIntensity = 0.0;
    this->initialized = false;
}
