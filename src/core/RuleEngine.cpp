#include "RuleEngine.h"

/**
 * @brief 构造函数
 */
RuleEngine::RuleEngine() {
}

/**
 * @brief 析构函数
 */
RuleEngine::~RuleEngine() {
    // 释放所有规则内存
    for (auto rule : rules) {
        delete rule;
    }
    rules.clear();
}

/**
 * @brief 添加规则到引擎
 * @param rule 规则指针
 * @return 成功返回true，失败返回false
 */
bool RuleEngine::addRule(Rule* rule) {
    if (rule == nullptr) {
        return false;
    }
    
    // 检查是否已存在相同ID的规则
    for (const auto& existingRule : rules) {
        if (existingRule->getId() == rule->getId()) {
            return false;
        }
    }
    
    rules.push_back(rule);
    return true;
}

/**
 * @brief 从引擎中移除规则
 * @param ruleId 规则ID
 * @return 成功返回true，失败返回false
 */
bool RuleEngine::removeRule(String ruleId) {
    for (auto it = rules.begin(); it != rules.end(); ++it) {
        if ((*it)->getId() == ruleId) {
            delete *it;
            rules.erase(it);
            return true;
        }
    }
    return false;
}

/**
 * @brief 获取指定ID的规则
 * @param ruleId 规则ID
 * @return 规则指针，若未找到返回nullptr
 */
Rule* RuleEngine::getRule(String ruleId) {
    for (auto rule : rules) {
        if (rule->getId() == ruleId) {
            return rule;
        }
    }
    return nullptr;
}

/**
 * @brief 获取所有规则列表
 * @return 包含所有规则指针的vector
 */
std::vector<Rule*> RuleEngine::getAllRules() {
    return rules;
}

/**
 * @brief 启用规则
 * @param ruleId 规则ID
 * @return 成功返回true，失败返回false
 */
bool RuleEngine::enableRule(String ruleId) {
    Rule* rule = getRule(ruleId);
    if (rule != nullptr) {
        rule->setEnabled(true);
        return true;
    }
    return false;
}

/**
 * @brief 禁用规则
 * @param ruleId 规则ID
 * @return 成功返回true，失败返回false
 */
bool RuleEngine::disableRule(String ruleId) {
    Rule* rule = getRule(ruleId);
    if (rule != nullptr) {
        rule->setEnabled(false);
        return true;
    }
    return false;
}

/**
 * @brief 评估所有规则并执行满足条件的规则动作
 * @param deviceManager 设备管理器
 * @param envManager 环境传感器管理器
 * @return 成功执行的规则数量
 */
int RuleEngine::evaluateRules(DeviceManager& deviceManager, EnvironmentManager& envManager) {
    int executedRulesCount = 0;
    
    for (auto rule : rules) {
        // 评估规则条件
        if (rule->evaluateConditions(envManager)) {
            // 执行规则动作
            if (rule->executeActions(deviceManager)) {
                executedRulesCount++;
            }
        }
    }
    
    return executedRulesCount;
}

/**
 * @brief 保存所有规则到存储
 * @return 成功返回true，失败返回false
 */
bool RuleEngine::saveRules() {
    // TODO: 实现规则保存到EEPROM或SPIFFS的功能
    return false;
}

/**
 * @brief 从存储加载所有规则
 * @return 成功返回true，失败返回false
 */
bool RuleEngine::loadRules() {
    // TODO: 实现从EEPROM或SPIFFS加载规则的功能
    return false;
}

/**
 * @brief 获取规则数量
 * @return 规则数量
 */
int RuleEngine::getRuleCount() {
    return rules.size();
}
