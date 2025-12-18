#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include "EnvironmentSensor.h"

// BH1750 相关定义
#define BH1750_ADDR_H 0x5C
#define BH1750_ADDR_L 0x23
#define BH1750_POWER_DOWN 0x00
#define BH1750_POWER_ON 0x01
#define BH1750_RESET 0x07
#define BH1750_CONT_H_RES 0x10
#define BH1750_CONT_H_RES2 0x11
#define BH1750_CONT_L_RES 0x13
#define BH1750_ONE_H_RES 0x20
#define BH1750_ONE_H_RES2 0x21
#define BH1750_ONE_L_RES 0x23

class LightSensor : public EnvironmentSensor {
public:
    LightSensor(String sensorId, uint8_t i2cAddr = BH1750_ADDR_L);
    LightSensor(String sensorId, int analogPin);
    ~LightSensor();

    bool initialize() override;
    float readData() override;

    String getType() override;
    String getId() override;
    int getPin() override;
    String getUnit() override;

    bool calibrate() override;
    void setMode(uint8_t mode);

private:
    String sensorId;
    uint8_t i2cAddr;
    uint8_t measurementMode;
    bool useAnalog;
    int analogPin;
    float lastLightIntensity;
    bool initialized;
    bool sendCommand(uint8_t cmd);
};

#endif // LIGHT_SENSOR_H
