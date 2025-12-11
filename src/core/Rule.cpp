#include "Rule.h"

/**
 * @brief 构造函数
 * @param id 规则ID
 * @param name 规则名称
 * @param enabled 规则是否启用
 */
Rule::Rule(String id, String name, bool enabled) : id(id), name(name), enabled(enabled) {
}

/**
 * @brief 析构函数
 */
Rule::~Rule() {
    conditions.clear();
    actions.clear();
}

/**
 * @brief 添加规则条件
 * @param condition 规则条件
 */
void Rule::addCondition(const RuleCondition& condition) {
    conditions.push_back(condition);
}

/**
 * @brief 添加规则动作
 * @param action 规则动作
 */
void Rule::addAction(const RuleAction& action) {
    actions.push_back(action);
}

/**
 * @brief 评估规则条件是否满足
 * @param envManager 环境传感器管理器
 * @return 条件满足返回true，否则返回false
 */
bool Rule::evaluateConditions(EnvironmentManager& envManager) {
    // 如果规则未启用，直接返回false
    if (!enabled) {
        return false;
    }
    
    // 收集所有传感器数据
    envManager.collectAllSensorData();
    
    // 评估所有条件，只有当所有条件都满足时才返回true
    for (const auto& condition : conditions) {
        // 获取传感器当前值
        float currentValue = envManager.collectSensorData(condition.sensorId);
        
        // 如果传感器不存在，跳过此条件
        if (isnan(currentValue)) {
            continue;
        }
        
        // 评估条件
        bool conditionMet = false;
        if (condition.operatorType == "<") {
            conditionMet = (currentValue < condition.threshold);
        } else if (condition.operatorType == "<=") {
            conditionMet = (currentValue <= condition.threshold);
        } else if (condition.operatorType == ">") {
            conditionMet = (currentValue > condition.threshold);
        } else if (condition.operatorType == ">=") {
            conditionMet = (currentValue >= condition.threshold);
        } else if (condition.operatorType == "==") {
            conditionMet = (currentValue == condition.threshold);
        } else if (condition.operatorType == "!=") {
            conditionMet = (currentValue != condition.threshold);
        }
        
        // 如果有任何一个条件不满足，返回false
        if (!conditionMet) {
            return false;
        }
    }
    
    // 所有条件都满足，返回true
    return true;
}

/**
 * @brief 执行规则动作
 * @param deviceManager 设备管理器
 * @return 动作执行成功返回true，否则返回false
 */
bool Rule::executeActions(DeviceManager& deviceManager) {
    bool allActionsSuccess = true;
    
    // 执行所有动作
    for (const auto& action : actions) {
        // 更新设备状态
        bool actionSuccess = deviceManager.updateDeviceStatus(action.deviceId, action.targetStatus);
        
        // 如果有任何一个动作失败，记录失败状态
        if (!actionSuccess) {
            allActionsSuccess = false;
        }
    }
    
    return allActionsSuccess;
}

/**
 * @brief 获取规则ID
 * @return 规则ID
 */
String Rule::getId() {
    return id;
}

/**
 * @brief 获取规则名称
 * @return 规则名称
 */
String Rule::getName() {
    return name;
}

/**
 * @brief 检查规则是否启用
 * @return 启用返回true，否则返回false
 */
bool Rule::isEnabled() {
    return enabled;
}

/**
 * @brief 设置规则是否启用
 * @param enabled 启用状态
 */
void Rule::setEnabled(bool enabled) {
    this->enabled = enabled;
}