#include "AlarmManager.h"

/**
 * @brief 构造函数
 * @param dataLogger 数据记录器指针
 */
AlarmManager::AlarmManager(DataLogger* dataLogger, SPIFFSStorage* storage) : dataLogger(dataLogger), storage(storage) {
}

/**
 * @brief 析构函数
 */
AlarmManager::~AlarmManager() {
}

void AlarmManager::setBuzzer(String id) {
    buzzerId = id;
}

void AlarmManager::muteBuzzerUntilNormal() {
    buzzerMuted = true;
}

void AlarmManager::resetBuzzerMute() {
    buzzerMuted = false;
}

bool AlarmManager::syncBuzzer(DeviceManager& deviceManager) {
    bool smokeActive = false;
    for (const auto& a : activeAlarms) {
        if (a.sensorId == "smoke" && a.isActive) {
            smokeActive = true;
            break;
        }
    }
    AlarmThreshold th = getThreshold("smoke");
    bool shouldOn = th.enabled && smokeActive && !buzzerMuted;
    bool changed = (shouldOn != lastBuzzerState);
    if (buzzerId.length() > 0 && changed) {
        deviceManager.updateDeviceStatus(buzzerId, shouldOn);
        lastBuzzerState = shouldOn;
    }
    if (!smokeActive) {
        buzzerMuted = false;
    }
    return changed;
}
/**
 * @brief 添加告警阈值
 * @param threshold 告警阈值
 * @return 成功返回true，失败返回false
 */
bool AlarmManager::addThreshold(const AlarmThreshold& threshold) {
    // 检查是否已存在相同传感器ID的阈值
    for (auto& existingThreshold : thresholds) {
        if (existingThreshold.sensorId == threshold.sensorId) {
            // 更新现有阈值
            existingThreshold = threshold;
            return true;
        }
    }
    
    // 添加新阈值
    thresholds.push_back(threshold);
    return true;
}

bool AlarmManager::loadFromStorage() {
    if (storage == nullptr) {
        return false;
    }
    if (!storage->init()) {
        return false;
    }
    DynamicJsonDocument doc(1024);
    if (!storage->readJsonFile(SPIFFS_CONFIG_FILE, doc)) {
        return false;
    }
    JsonObject alarms = doc["alarms"].as<JsonObject>();
    if (!alarms.isNull()) {
        for (JsonPair kv : alarms) {
            String sid = String(kv.key().c_str());
            JsonObject o = kv.value().as<JsonObject>();
            AlarmThreshold th;
            th.sensorId = sid;
            th.minThreshold = o.containsKey("minThreshold") ? o["minThreshold"].as<float>() : -1;
            th.maxThreshold = o.containsKey("maxThreshold") ? o["maxThreshold"].as<float>() : -1;
            th.enabled = o.containsKey("enabled") ? o["enabled"].as<bool>() : false;
            addThreshold(th);
        }
    }
    return true;
}

bool AlarmManager::saveToStorage() {
    if (storage == nullptr) {
        return false;
    }
    if (!storage->init()) {
        return false;
    }
    DynamicJsonDocument doc(1024);
    // 读取现有配置，避免覆盖其他字段
    storage->readJsonFile(SPIFFS_CONFIG_FILE, doc);
    JsonObject alarms = doc.containsKey("alarms") ? doc["alarms"].as<JsonObject>() : doc.createNestedObject("alarms");
    // 写入所有阈值
    for (const auto& th : thresholds) {
        JsonObject o = alarms.containsKey(th.sensorId) ? alarms[th.sensorId].as<JsonObject>() : alarms.createNestedObject(th.sensorId);
        o["minThreshold"] = th.minThreshold;
        o["maxThreshold"] = th.maxThreshold;
        o["enabled"] = th.enabled;
    }
    return storage->writeJsonFile(SPIFFS_CONFIG_FILE, doc);
}
/**
 * @brief 更新告警阈值
 * @param threshold 告警阈值
 * @return 成功返回true，失败返回false
 */
bool AlarmManager::updateThreshold(const AlarmThreshold& threshold) {
    // 调用addThreshold，它已经包含了更新逻辑
    bool ok = addThreshold(threshold);
    if (ok) {
        saveToStorage();
    }
    return ok;
}

/**
 * @brief 移除告警阈值
 * @param sensorId 传感器ID
 * @return 成功返回true，失败返回false
 */
bool AlarmManager::removeThreshold(String sensorId) {
    for (auto it = thresholds.begin(); it != thresholds.end(); ++it) {
        if (it->sensorId == sensorId) {
            thresholds.erase(it);
            return true;
        }
    }
    return false;
}

/**
 * @brief 获取指定传感器的告警阈值
 * @param sensorId 传感器ID
 * @return 告警阈值结构体，若未找到返回默认值
 */
AlarmThreshold AlarmManager::getThreshold(String sensorId) {
    for (const auto& threshold : thresholds) {
        if (threshold.sensorId == sensorId) {
            return threshold;
        }
    }
    
    // 返回默认值
    AlarmThreshold defaultThreshold;
    defaultThreshold.sensorId = sensorId;
    defaultThreshold.minThreshold = -1;
    defaultThreshold.maxThreshold = -1;
    defaultThreshold.enabled = false;
    return defaultThreshold;
}

/**
 * @brief 添加告警联动动作
 * @param action 告警联动动作
 */
void AlarmManager::addAlarmAction(const AlarmAction& action) {
    alarmActions.push_back(action);
}

/**
 * @brief 检查传感器数据是否触发告警
 * @param envManager 环境传感器管理器
 * @return 触发的告警状态列表
 */
std::vector<AlarmStatus> AlarmManager::checkAlarms(EnvironmentManager& envManager) {
    std::vector<AlarmStatus> triggeredAlarms;
    
    // 收集所有传感器数据
    envManager.collectAllSensorData();
    
    // 检查每个传感器的阈值
    for (const auto& threshold : thresholds) {
        // 如果阈值未启用，跳过
        if (!threshold.enabled) {
            continue;
        }
        
        // 获取传感器当前值
        float currentValue = envManager.collectSensorData(threshold.sensorId);
        
        // 如果传感器不存在，跳过
        if (isnan(currentValue)) {
            continue;
        }
        
        // 检查是否超出阈值范围
        bool isAlarmActive = false;
        String message = "";
        
        if (threshold.minThreshold != -1 && currentValue < threshold.minThreshold) {
            isAlarmActive = true;
            message = "传感器" + threshold.sensorId + "值低于最小阈值! 当前值: " + String(currentValue) + ", 阈值: " + String(threshold.minThreshold);
        } else if (threshold.maxThreshold != -1 && currentValue > threshold.maxThreshold) {
            isAlarmActive = true;
            message = "传感器" + threshold.sensorId + "值高于最大阈值! 当前值: " + String(currentValue) + ", 阈值: " + String(threshold.maxThreshold);
        }
        
        // 检查是否已有该传感器的告警
        bool alarmExists = false;
        for (auto& activeAlarm : activeAlarms) {
            if (activeAlarm.sensorId == threshold.sensorId) {
                alarmExists = true;
                
                if (isAlarmActive) {
                    // 更新现有告警
                    activeAlarm.isActive = true;
                    activeAlarm.currentValue = currentValue;
                    activeAlarm.message = message;
                    triggeredAlarms.push_back(activeAlarm);
                    
                    // 记录告警到日志
                    if (dataLogger != nullptr) {
                        dataLogger->log("ALARM", message);
                    }
                } else {
                    // 清除已解决的告警
                    activeAlarm.isActive = false;
                    
                    // 记录告警解除到日志
                    if (dataLogger != nullptr) {
                        String resolveMessage = "传感器" + threshold.sensorId + "告警已解除! 当前值: " + String(currentValue);
                        dataLogger->log("ALARM", resolveMessage);
                    }
                }
                break;
            }
        }
        
        // 如果没有现有告警且当前是告警状态，创建新告警
        if (isAlarmActive && !alarmExists) {
            AlarmStatus newAlarm;
            newAlarm.sensorId = threshold.sensorId;
            newAlarm.isActive = true;
            newAlarm.currentValue = currentValue;
            newAlarm.message = message;
            newAlarm.time = millis();
            
            activeAlarms.push_back(newAlarm);
            triggeredAlarms.push_back(newAlarm);
            
            // 记录告警到日志
            if (dataLogger != nullptr) {
                dataLogger->log("ALARM", message);
            }
        }
    }
    
    return triggeredAlarms;
}

/**
 * @brief 执行告警联动动作
 * @param deviceManager 设备管理器
 * @return 动作执行成功返回true，否则返回false
 */
bool AlarmManager::executeAlarmActions(DeviceManager& deviceManager) {
    bool allActionsSuccess = true;
    
    // 执行所有告警联动动作
    for (const auto& action : alarmActions) {
        bool actionSuccess = deviceManager.updateDeviceStatus(action.deviceId, action.targetStatus);
        
        // 如果有任何一个动作失败，记录失败状态
        if (!actionSuccess) {
            allActionsSuccess = false;
        }
    }
    
    return allActionsSuccess;
}

/**
 * @brief 获取当前激活的告警
 * @return 激活的告警状态列表
 */
std::vector<AlarmStatus> AlarmManager::getActiveAlarms() {
    std::vector<AlarmStatus> result;
    
    for (const auto& alarm : activeAlarms) {
        if (alarm.isActive) {
            result.push_back(alarm);
        }
    }
    
    return result;
}

/**
 * @brief 清除指定传感器的告警
 * @param sensorId 传感器ID
 * @return 成功返回true，失败返回false
 */
bool AlarmManager::clearAlarm(String sensorId) {
    for (auto& alarm : activeAlarms) {
        if (alarm.sensorId == sensorId) {
            alarm.isActive = false;
            return true;
        }
    }
    return false;
}

/**
 * @brief 清除所有告警
 * @return 成功返回true，失败返回false
 */
bool AlarmManager::clearAllAlarms() {
    for (auto& alarm : activeAlarms) {
        alarm.isActive = false;
    }
    return true;
}
