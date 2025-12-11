#include "EEPROMStorage.h"

EEPROMStorage::EEPROMStorage() {
    _initialized = false;
}

EEPROMStorage::~EEPROMStorage() {
    if (_initialized) {
        EEPROM.end();
    }
}

bool EEPROMStorage::init() {
    if (!_initialized) {
        // 初始化EEPROM
        if (!EEPROM.begin(EEPROM_TOTAL_SIZE)) {
            return false;
        }
        _initialized = true;
    }
    return true;
}

bool EEPROMStorage::readSystemConfig(SystemConfig &config) {
    if (!_initialized) {
        return false;
    }
    
    // 读取系统配置
    EEPROM.get(EEPROM_SYSTEM_CONFIG_ADDRESS, config);
    
    // 验证配置是否有效（简单验证，检查是否有默认值）
    if (config.sensorUpdateInterval == 0) {
        config.sensorUpdateInterval = DEFAULT_SENSOR_UPDATE_INTERVAL;
    }
    
    if (config.devicePollingInterval == 0) {
        config.devicePollingInterval = DEFAULT_DEVICE_POLLING_INTERVAL;
    }
    
    return true;
}

bool EEPROMStorage::writeSystemConfig(const SystemConfig &config) {
    if (!_initialized) {
        return false;
    }
    
    // 写入系统配置
    EEPROM.put(EEPROM_SYSTEM_CONFIG_ADDRESS, config);
    return EEPROM.commit();
}

bool EEPROMStorage::readDeviceState(const String &deviceId, DeviceState &state) {
    if (!_initialized) {
        return false;
    }
    
    int index = -1;
    if (!findDeviceStateIndex(deviceId, index)) {
        return false;
    }
    
    // 读取设备状态的固定部分
    int address = EEPROM_DEVICE_STATES_ADDRESS + index * sizeof(DeviceState);
    
    // 读取pin、status、brightness
    EEPROM.get(address, state.pin);
    address += sizeof(state.pin);
    
    EEPROM.get(address, state.status);
    address += sizeof(state.status);
    
    EEPROM.get(address, state.brightness);
    address += sizeof(state.brightness);
    
    // 读取deviceId和deviceType字符串
    address += readString(address, state.deviceId);
    readString(address, state.deviceType);
    
    return true;
}

bool EEPROMStorage::writeDeviceState(const DeviceState &state) {
    if (!_initialized) {
        return false;
    }
    
    int index = -1;
    if (!findDeviceStateIndex(state.deviceId, index)) {
        // 如果找不到，找一个空的位置
        for (int i = 0; i < 10; i++) {
            String tempId;
            int address = EEPROM_DEVICE_STATES_ADDRESS + i * sizeof(DeviceState) + sizeof(int) * 3;
            readString(address, tempId);
            if (tempId.isEmpty()) {
                index = i;
                break;
            }
        }
    }
    
    if (index == -1) {
        // 没有找到空位置
        return false;
    }
    
    // 写入设备状态
    int address = EEPROM_DEVICE_STATES_ADDRESS + index * sizeof(DeviceState);
    
    // 写入pin、status、brightness
    EEPROM.put(address, state.pin);
    address += sizeof(state.pin);
    
    EEPROM.put(address, state.status);
    address += sizeof(state.status);
    
    EEPROM.put(address, state.brightness);
    address += sizeof(state.brightness);
    
    // 写入deviceId和deviceType字符串
    address += writeString(address, state.deviceId);
    writeString(address, state.deviceType);
    
    return EEPROM.commit();
}

bool EEPROMStorage::readAllDeviceStates(DeviceState *states, int &count) {
    if (!_initialized || states == nullptr) {
        return false;
    }
    
    count = 0;
    
    for (int i = 0; i < 10; i++) {
        DeviceState state;
        int address = EEPROM_DEVICE_STATES_ADDRESS + i * sizeof(DeviceState);
        
        // 读取pin、status、brightness
        EEPROM.get(address, state.pin);
        address += sizeof(state.pin);
        
        EEPROM.get(address, state.status);
        address += sizeof(state.status);
        
        EEPROM.get(address, state.brightness);
        address += sizeof(state.brightness);
        
        // 读取deviceId和deviceType字符串
        address += readString(address, state.deviceId);
        readString(address, state.deviceType);
        
        // 如果deviceId不为空，说明是一个有效的设备状态
        if (!state.deviceId.isEmpty()) {
            states[count] = state;
            count++;
        }
    }
    
    return true;
}

bool EEPROMStorage::readRule(const String &ruleId, Rule &rule) {
    if (!_initialized) {
        return false;
    }
    
    int index = -1;
    if (!findRuleIndex(ruleId, index)) {
        return false;
    }
    
    // 读取规则的固定部分
    int address = EEPROM_RULES_ADDRESS + index * sizeof(Rule);
    
    // 读取enabled
    EEPROM.get(address, rule.enabled);
    address += sizeof(rule.enabled);
    
    // 读取条件部分
    address += readString(address, rule.condition.sensorType);
    address += readString(address, rule.condition.operator);
    EEPROM.get(address, rule.condition.threshold);
    address += sizeof(rule.condition.threshold);
    
    // 读取动作部分
    address += readString(address, rule.action.deviceId);
    address += readString(address, rule.action.actionType);
    EEPROM.get(address, rule.action.value);
    address += sizeof(rule.action.value);
    
    // 读取规则基本信息
    address += readString(address, rule.ruleId);
    address += readString(address, rule.ruleName);
    readString(address, rule.description);
    
    return true;
}

bool EEPROMStorage::writeRule(const Rule &rule) {
    if (!_initialized) {
        return false;
    }
    
    int index = -1;
    if (!findRuleIndex(rule.ruleId, index)) {
        // 如果找不到，找一个空的位置
        for (int i = 0; i < 10; i++) {
            String tempId;
            int address = EEPROM_RULES_ADDRESS + i * sizeof(Rule) + sizeof(bool) + sizeof(float) + sizeof(int);
            // 跳过enabled、threshold、value
            address += readString(address, tempId); // sensorType
            address += readString(address, tempId); // operator
            address += readString(address, tempId); // deviceId
            address += readString(address, tempId); // actionType
            readString(address, tempId); // ruleId
            if (tempId.isEmpty()) {
                index = i;
                break;
            }
        }
    }
    
    if (index == -1) {
        // 没有找到空位置
        return false;
    }
    
    // 写入规则
    int address = EEPROM_RULES_ADDRESS + index * sizeof(Rule);
    
    // 写入enabled
    EEPROM.put(address, rule.enabled);
    address += sizeof(rule.enabled);
    
    // 写入条件部分
    address += writeString(address, rule.condition.sensorType);
    address += writeString(address, rule.condition.operator);
    EEPROM.put(address, rule.condition.threshold);
    address += sizeof(rule.condition.threshold);
    
    // 写入动作部分
    address += writeString(address, rule.action.deviceId);
    address += writeString(address, rule.action.actionType);
    EEPROM.put(address, rule.action.value);
    address += sizeof(rule.action.value);
    
    // 写入规则基本信息
    address += writeString(address, rule.ruleId);
    address += writeString(address, rule.ruleName);
    writeString(address, rule.description);
    
    return EEPROM.commit();
}

bool EEPROMStorage::readAllRules(Rule *rules, int &count) {
    if (!_initialized || rules == nullptr) {
        return false;
    }
    
    count = 0;
    
    for (int i = 0; i < 10; i++) {
        Rule rule;
        int address = EEPROM_RULES_ADDRESS + i * sizeof(Rule);
        
        // 读取enabled
        EEPROM.get(address, rule.enabled);
        address += sizeof(rule.enabled);
        
        // 读取条件部分
        address += readString(address, rule.condition.sensorType);
        address += readString(address, rule.condition.operator);
        EEPROM.get(address, rule.condition.threshold);
        address += sizeof(rule.condition.threshold);
        
        // 读取动作部分
        address += readString(address, rule.action.deviceId);
        address += readString(address, rule.action.actionType);
        EEPROM.get(address, rule.action.value);
        address += sizeof(rule.action.value);
        
        // 读取规则基本信息
        address += readString(address, rule.ruleId);
        address += readString(address, rule.ruleName);
        readString(address, rule.description);
        
        // 如果ruleId不为空，说明是一个有效的规则
        if (!rule.ruleId.isEmpty()) {
            rules[count] = rule;
            count++;
        }
    }
    
    return true;
}

bool EEPROMStorage::readAlarmSetting(const String &alarmType, AlarmSetting &setting) {
    if (!_initialized) {
        return false;
    }
    
    int index = -1;
    if (!findAlarmSettingIndex(alarmType, index)) {
        return false;
    }
    
    // 读取告警设置
    int address = EEPROM_ALARM_SETTINGS_ADDRESS + index * sizeof(AlarmSetting);
    
    // 读取enabled、threshold、duration
    EEPROM.get(address, setting.enabled);
    address += sizeof(setting.enabled);
    
    EEPROM.get(address, setting.threshold);
    address += sizeof(setting.threshold);
    
    EEPROM.get(address, setting.duration);
    address += sizeof(setting.duration);
    
    // 读取alarmType和deviceId字符串
    address += readString(address, setting.alarmType);
    readString(address, setting.deviceId);
    
    return true;
}

bool EEPROMStorage::writeAlarmSetting(const AlarmSetting &setting) {
    if (!_initialized) {
        return false;
    }
    
    int index = -1;
    if (!findAlarmSettingIndex(setting.alarmType, index)) {
        // 如果找不到，找一个空的位置
        for (int i = 0; i < 10; i++) {
            String tempType;
            int address = EEPROM_ALARM_SETTINGS_ADDRESS + i * sizeof(AlarmSetting) + sizeof(bool) + sizeof(float) + sizeof(int);
            readString(address, tempType);
            if (tempType.isEmpty()) {
                index = i;
                break;
            }
        }
    }
    
    if (index == -1) {
        // 没有找到空位置
        return false;
    }
    
    // 写入告警设置
    int address = EEPROM_ALARM_SETTINGS_ADDRESS + index * sizeof(AlarmSetting);
    
    // 写入enabled、threshold、duration
    EEPROM.put(address, setting.enabled);
    address += sizeof(setting.enabled);
    
    EEPROM.put(address, setting.threshold);
    address += sizeof(setting.threshold);
    
    EEPROM.put(address, setting.duration);
    address += sizeof(setting.duration);
    
    // 写入alarmType和deviceId字符串
    address += writeString(address, setting.alarmType);
    writeString(address, setting.deviceId);
    
    return EEPROM.commit();
}

bool EEPROMStorage::readAllAlarmSettings(AlarmSetting *settings, int &count) {
    if (!_initialized || settings == nullptr) {
        return false;
    }
    
    count = 0;
    
    for (int i = 0; i < 10; i++) {
        AlarmSetting setting;
        int address = EEPROM_ALARM_SETTINGS_ADDRESS + i * sizeof(AlarmSetting);
        
        // 读取enabled、threshold、duration
        EEPROM.get(address, setting.enabled);
        address += sizeof(setting.enabled);
        
        EEPROM.get(address, setting.threshold);
        address += sizeof(setting.threshold);
        
        EEPROM.get(address, setting.duration);
        address += sizeof(setting.duration);
        
        // 读取alarmType和deviceId字符串
        address += readString(address, setting.alarmType);
        readString(address, setting.deviceId);
        
        // 如果alarmType不为空，说明是一个有效的告警设置
        if (!setting.alarmType.isEmpty()) {
            settings[count] = setting;
            count++;
        }
    }
    
    return true;
}

bool EEPROMStorage::clear() {
    if (!_initialized) {
        return false;
    }
    
    // 清空EEPROM
    for (int i = 0; i < EEPROM_TOTAL_SIZE; i++) {
        EEPROM.write(i, 0);
    }
    
    return EEPROM.commit();
}

bool EEPROMStorage::findDeviceStateIndex(const String &deviceId, int &index) {
    for (int i = 0; i < 10; i++) {
        String tempId;
        int address = EEPROM_DEVICE_STATES_ADDRESS + i * sizeof(DeviceState) + sizeof(int) * 3;
        readString(address, tempId);
        if (tempId == deviceId) {
            index = i;
            return true;
        }
    }
    return false;
}

bool EEPROMStorage::findRuleIndex(const String &ruleId, int &index) {
    for (int i = 0; i < 10; i++) {
        String tempId;
        int address = EEPROM_RULES_ADDRESS + i * sizeof(Rule) + sizeof(bool) + sizeof(float) + sizeof(int);
        // 跳过enabled、threshold、value
        address += readString(address, tempId); // sensorType
        address += readString(address, tempId); // operator
        address += readString(address, tempId); // deviceId
        address += readString(address, tempId); // actionType
        readString(address, tempId); // ruleId
        if (tempId == ruleId) {
            index = i;
            return true;
        }
    }
    return false;
}

bool EEPROMStorage::findAlarmSettingIndex(const String &alarmType, int &index) {
    for (int i = 0; i < 10; i++) {
        String tempType;
        int address = EEPROM_ALARM_SETTINGS_ADDRESS + i * sizeof(AlarmSetting) + sizeof(bool) + sizeof(float) + sizeof(int);
        readString(address, tempType);
        if (tempType == alarmType) {
            index = i;
            return true;
        }
    }
    return false;
}

int EEPROMStorage::readString(int address, String &str) {
    int length = EEPROM.read(address);
    address++;
    
    if (length > 0) {
        char buffer[100]; // 最大字符串长度为100
        for (int i = 0; i < length && i < 99; i++) {
            buffer[i] = EEPROM.read(address + i);
        }
        buffer[length] = '\0';
        str = String(buffer);
    } else {
        str = "";
    }
    
    return length + 1; // 返回读取的总字节数（包括长度字节）
}

int EEPROMStorage::writeString(int address, const String &str) {
    int length = str.length();
    if (length > 99) length = 99; // 限制字符串最大长度为99
    
    // 写入字符串长度
    EEPROM.write(address, length);
    address++;
    
    // 写入字符串内容
    for (int i = 0; i < length; i++) {
        EEPROM.write(address + i, str.charAt(i));
    }
    
    return length + 1; // 返回写入的总字节数（包括长度字节）
}
