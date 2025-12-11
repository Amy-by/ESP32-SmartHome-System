#include "EnvironmentManager.h"

EnvironmentManager::EnvironmentManager() {
    // 初始化传感器列表为空
    sensors.clear();
}

EnvironmentManager::~EnvironmentManager() {
    // 释放所有传感器指针
    for (auto sensor : sensors) {
        delete sensor;
    }
    sensors.clear();
}

bool EnvironmentManager::addSensor(EnvironmentSensor* sensor) {
    if (sensor == nullptr) {
        return false;
    }
    
    // 检查传感器ID是否已存在
    for (auto existingSensor : sensors) {
        if (existingSensor->getId() == sensor->getId()) {
            return false;
        }
    }
    
    // 添加传感器到列表
    sensors.push_back(sensor);
    return true;
}

bool EnvironmentManager::removeSensor(String sensorId) {
    for (auto it = sensors.begin(); it != sensors.end(); ++it) {
        if ((*it)->getId() == sensorId) {
            delete *it;
            sensors.erase(it);
            return true;
        }
    }
    return false;
}

EnvironmentSensor* EnvironmentManager::getSensor(String sensorId) {
    for (auto sensor : sensors) {
        if (sensor->getId() == sensorId) {
            return sensor;
        }
    }
    return nullptr;
}

std::vector<EnvironmentSensor*> EnvironmentManager::getAllSensors() {
    return sensors;
}

bool EnvironmentManager::collectAllSensorData() {
    bool allSuccess = true;
    for (auto sensor : sensors) {
        if (sensor == nullptr) {
            continue;
        }
        // 采集数据，如果失败记录但继续采集其他传感器
        if (isnan(sensor->readData())) {
            allSuccess = false;
        }
    }
    return allSuccess;
}

float EnvironmentManager::collectSensorData(String sensorId) {
    EnvironmentSensor* sensor = getSensor(sensorId);
    if (sensor == nullptr) {
        return NAN;
    }
    return sensor->readData();
}

bool EnvironmentManager::initializeAllSensors() {
    bool allSuccess = true;
    for (auto sensor : sensors) {
        if (sensor == nullptr) {
            continue;
        }
        // 初始化传感器，如果失败记录但继续初始化其他传感器
        if (!sensor->initialize()) {
            allSuccess = false;
        }
    }
    return allSuccess;
}

int EnvironmentManager::getSensorCount() {
    return sensors.size();
}

bool EnvironmentManager::saveSensorConfigurations() {
    // TODO: 实现保存传感器配置到EEPROM或SPIFFS
    // 暂时返回true表示成功
    return true;
}

bool EnvironmentManager::loadSensorConfigurations() {
    // TODO: 实现从EEPROM或SPIFFS加载传感器配置
    // 暂时返回true表示成功
    return true;
}
