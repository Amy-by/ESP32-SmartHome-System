#ifndef RULEENGINE_H
#define RULEENGINE_H

#include <Arduino.h>
#include <vector>
#include "Rule.h"
#include "devices/DeviceManager.h"
#include "sensors/EnvironmentManager.h"

/**
 * @brief 规则引擎类，负责管理和评估所有规则
 * 提供规则的添加、移除、启用、禁用等功能，并能够评估所有规则
 */
class RuleEngine {
public:
    /**
     * @brief 构造函数
     */
    RuleEngine();
    
    /**
     * @brief 析构函数
     */
    ~RuleEngine();
    
    /**
     * @brief 添加规则到引擎
     * @param rule 规则指针
     * @return 成功返回true，失败返回false
     */
    bool addRule(Rule* rule);
    
    /**
     * @brief 从引擎中移除规则
     * @param ruleId 规则ID
     * @return 成功返回true，失败返回false
     */
    bool removeRule(String ruleId);
    
    /**
     * @brief 获取指定ID的规则
     * @param ruleId 规则ID
     * @return 规则指针，若未找到返回nullptr
     */
    Rule* getRule(String ruleId);
    
    /**
     * @brief 获取所有规则列表
     * @return 包含所有规则指针的vector
     */
    std::vector<Rule*> getAllRules();
    
    /**
     * @brief 启用规则
     * @param ruleId 规则ID
     * @return 成功返回true，失败返回false
     */
    bool enableRule(String ruleId);
    
    /**
     * @brief 禁用规则
     * @param ruleId 规则ID
     * @return 成功返回true，失败返回false
     */
    bool disableRule(String ruleId);
    
    /**
     * @brief 评估所有规则并执行满足条件的规则动作
     * @param deviceManager 设备管理器
     * @param envManager 环境传感器管理器
     * @return 成功执行的规则数量
     */
    int evaluateRules(DeviceManager& deviceManager, EnvironmentManager& envManager);
    
    /**
     * @brief 保存所有规则到存储
     * @return 成功返回true，失败返回false
     */
    bool saveRules();
    
    /**
     * @brief 从存储加载所有规则
     * @return 成功返回true，失败返回false
     */
    bool loadRules();
    
    /**
     * @brief 获取规则数量
     * @return 规则数量
     */
    int getRuleCount();
    
private:
    // 规则列表
    std::vector<Rule*> rules;
};

#endif // RULEENGINE_H