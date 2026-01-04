#include "RuleEngine.h"

/**
 * @brief 构造函数
 * @param dbManager 数据库管理器对象
 */
RuleEngine::RuleEngine(DatabaseManager& dbManager) : dbManager(dbManager) {
    // 构造函数初始化
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
 * @brief 保存所有规则到SPIFFS存储
 * @return 成功返回true，失败返回false
 */
bool RuleEngine::saveRules() {
    // 保存所有规则到数据库
    for (auto rule : rules) {
        String ruleId = rule->getId();
        String ruleName = rule->getName();
        bool enabled = rule->isEnabled();
        const auto& conditions = rule->getConditions();
        const auto& actions = rule->getActions();
        
        // 尝试将字符串ID转换为整数
        int dbRuleId = ruleId.toInt();
        
        if (dbRuleId > 0) {
            // 更新现有规则
            if (!dbManager.updateRule(dbRuleId, ruleName, enabled, conditions, actions)) {
                Serial.printf("规则更新失败: ID = %s\n", ruleId.c_str());
                return false;
            }
        } else {
            // 创建新规则
            int newRuleId = dbManager.createRule(ruleName, enabled, conditions, actions);
            if (newRuleId > 0) {
                // 更新规则ID为数据库返回的ID
                rule->setId(String(newRuleId));
                Serial.printf("新规则创建成功，ID: %d, 原ID: %s\n", newRuleId, ruleId.c_str());
            } else {
                Serial.printf("新规则创建失败: 原ID = %s\n", ruleId.c_str());
                return false;
            }
        }
    }
    
    Serial.println("所有规则保存成功");
    return true;
}

/**
 * @brief 从SPIFFS存储加载所有规则
 * @return 成功返回true，失败返回false
 */
bool RuleEngine::loadRules() {
    // 清空现有规则
    for (auto rule : rules) {
        delete rule;
    }
    rules.clear();
    
    // 查询所有规则的基本信息
    String query = "SELECT * FROM rules ORDER BY id";
    String result;
    
    if (!dbManager.executeSelectQuery(query, result)) {
        Serial.println("查询规则列表失败");
        return false;
    }
    
    // 解析规则列表
    StaticJsonDocument<4096> rulesDoc;
    if (deserializeJson(rulesDoc, result) != DeserializationError::Ok) {
        Serial.println("解析规则列表失败");
        return false;
    }
    
    JsonArray rulesArray = rulesDoc["rows"];
    
    // 遍历每个规则
    for (JsonObject ruleObj : rulesArray) {
        int ruleId = ruleObj["id"].as<int>();
        String name = ruleObj["name"].as<String>();
        bool enabled = ruleObj["enabled"].as<int>() == 1;
        
        // 创建规则对象
        Rule* rule = new Rule(String(ruleId), name, enabled);
        
        // 查询规则的条件
        String conditionsQuery = "SELECT * FROM rule_conditions WHERE rule_id = " + String(ruleId);
        String conditionsResult;
        
        if (dbManager.executeSelectQuery(conditionsQuery, conditionsResult)) {
            StaticJsonDocument<2048> conditionsDoc;
            if (deserializeJson(conditionsDoc, conditionsResult) == DeserializationError::Ok) {
                JsonArray conditionsArray = conditionsDoc["rows"];
                
                for (JsonObject conditionObj : conditionsArray) {
                    RuleCondition condition;
                    condition.sensorId = conditionObj["sensor_id"].as<String>();
                    condition.threshold = conditionObj["threshold"].as<float>();
                    condition.operatorType = conditionObj["operator"].as<String>();
                    
                    rule->addCondition(condition);
                }
            }
        }
        
        // 查询规则的动作
        String actionsQuery = "SELECT * FROM rule_actions WHERE rule_id = " + String(ruleId);
        String actionsResult;
        
        if (dbManager.executeSelectQuery(actionsQuery, actionsResult)) {
            StaticJsonDocument<2048> actionsDoc;
            if (deserializeJson(actionsDoc, actionsResult) == DeserializationError::Ok) {
                JsonArray actionsArray = actionsDoc["rows"];
                
                for (JsonObject actionObj : actionsArray) {
                    RuleAction action;
                    action.deviceId = String(actionObj["device_id"].as<int>());
                    action.targetStatus = actionObj["target_status"].as<int>() == 1;
                    
                    rule->addAction(action);
                }
            }
        }
        
        // 将规则添加到规则引擎
        if (!addRule(rule)) {
            Serial.printf("规则添加失败：ID = %d\n", ruleId);
            delete rule;
        }
    }
    
    Serial.printf("规则加载完成，共加载 %d 条规则\n", rules.size());
    return true;
}

/**
 * @brief 获取规则数量
 * @return 规则数量
 */
int RuleEngine::getRuleCount() {
    return rules.size();
}
