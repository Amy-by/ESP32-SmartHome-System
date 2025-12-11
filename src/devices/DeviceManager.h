#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <Arduino.h>
#include <vector>
#include "Device.h"
#include "utils/GPIOController.h"

/**
 * @brief 设备管理器类，负责管理所有智能设备
 * 提供设备的添加、移除、状态查询和更新等功能
 */
class DeviceManager {
public:
    /**
     * @brief 构造函数，初始化设备管理器
     */
    DeviceManager();
    
    /**
     * @brief 析构函数
     */
    ~DeviceManager();
    
    /**
     * @brief 添加设备到管理器
     * @param device 设备指针
     * @return 成功返回true，失败返回false
     */
    bool addDevice(Device* device);
    
    /**
     * @brief 从管理器中移除设备
     * @param deviceId 设备ID
     * @return 成功返回true，失败返回false
     */
    bool removeDevice(String deviceId);
    
    /**
     * @brief 获取指定ID的设备指针
     * @param deviceId 设备ID
     * @return 设备指针，若未找到返回nullptr
     */
    Device* getDevice(String deviceId);
    
    /**
     * @brief 获取所有设备列表
     * @return 包含所有设备指针的vector
     */
    std::vector<Device*> getAllDevices();
    
    /**
     * @brief 更新设备状态
     * @param deviceId 设备ID
     * @param status 新状态
     * @return 成功返回true，失败返回false
     */
    bool updateDeviceStatus(String deviceId, bool status);
    
    /**
     * @brief 获取设备状态
     * @param deviceId 设备ID
     * @return 设备状态，若设备不存在返回false
     */
    bool getDeviceStatus(String deviceId);
    
    /**
     * @brief 保存所有设备状态到存储
     * @return 成功返回true，失败返回false
     */
    bool saveDeviceStates();
    
    /**
     * @brief 从存储加载设备状态
     * @return 成功返回true，失败返回false
     */
    bool loadDeviceStates();
    
    /**
     * @brief 获取设备数量
     * @return 设备数量
     */
    int getDeviceCount();
    
private:
    // 设备列表
    std::vector<Device*> devices;
    
    // GPIO控制器实例
    GPIOController gpioController;
};

#endif // DEVICEMANAGER_H