#include "DeviceManager.h"
#include <ArduinoJson.h>
#include "../include/config.h"

DeviceManager::DeviceManager() {
    // 初始化设备列表为空
    devices.clear();
}

DeviceManager::~DeviceManager() {
    // 释放所有设备指针
    for (auto device : devices) {
        delete device;
    }
    devices.clear();
}

bool DeviceManager::addDevice(Device* device) {
    if (device == nullptr) {
        return false;
    }
    
    // 检查设备ID是否已存在
    for (auto existingDevice : devices) {
        if (existingDevice->getId() == device->getId()) {
            return false;
        }
    }
    
    // 添加设备到列表
    devices.push_back(device);
    return true;
}

bool DeviceManager::removeDevice(String deviceId) {
    for (auto it = devices.begin(); it != devices.end(); ++it) {
        if ((*it)->getId() == deviceId) {
            delete *it;
            devices.erase(it);
            return true;
        }
    }
    return false;
}

Device* DeviceManager::getDevice(String deviceId) {
    for (auto device : devices) {
        if (device->getId() == deviceId) {
            return device;
        }
    }
    return nullptr;
}

std::vector<Device*> DeviceManager::getAllDevices() {
    return devices;
}

bool DeviceManager::updateDeviceStatus(String deviceId, bool status) {
    Device* device = getDevice(deviceId);
    if (device == nullptr) {
        return false;
    }
    return device->setStatus(status);
}

bool DeviceManager::getDeviceStatus(String deviceId) {
    Device* device = getDevice(deviceId);
    if (device == nullptr) {
        return false;
    }
    return device->getStatus();
}

bool DeviceManager::saveDeviceStates() {
    // TODO: 实现保存设备状态到EEPROM或SPIFFS
    // 暂时返回true表示成功
    return true;
}

bool DeviceManager::loadDeviceStates() {
    // TODO: 实现从EEPROM或SPIFFS加载设备状态
    // 暂时返回true表示成功
    return true;
}

int DeviceManager::getDeviceCount() {
    return devices.size();
}