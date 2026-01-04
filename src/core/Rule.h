#ifndef RULE_H
#define RULE_H

#include <Arduino.h>
#include <vector>
#include "devices/DeviceManager.h"
#include "sensors/EnvironmentManager.h"

/**
 * @brief 规则条件结构体，定义规则的触发条件
 */
struct RuleCondition {
    String sensorId;      // 传感器ID
    float threshold;      // 阈值
    String operatorType;  // 操作符："<", "<=", ">", ">=", "==", "!="
};

/**
 * @brief 规则动作结构体，定义规则触发后执行的动作
 */
struct RuleAction {
    String deviceId;      // 设备ID
    bool targetStatus;    // 目标状态：true表示开启，false表示关闭
};

/**
 * @brief 规则类，定义规则的结构和功能
 * 包含规则的条件定义、条件评估和动作执行功能
 */
class Rule {
public:
    /**
     * @brief 构造函数
     * @param id 规则ID
     * @param name 规则名称
     * @param enabled 规则是否启用
     */
    Rule(String id, String name, bool enabled = true);
    
    /**
     * @brief 析构函数
     */
    ~Rule();
    
    /**
     * @brief 添加规则条件
     * @param condition 规则条件
     */
    void addCondition(const RuleCondition& condition);
    
    /**
     * @brief 添加规则动作
     * @param action 规则动作
     */
    void addAction(const RuleAction& action);
    
    /**
     * @brief 评估规则条件是否满足
     * @param envManager 环境传感器管理器
     * @return 条件满足返回true，否则返回false
     */
    bool evaluateConditions(EnvironmentManager& envManager);
    
    /**
     * @brief 执行规则动作
     * @param deviceManager 设备管理器
     * @return 动作执行成功返回true，否则返回false
     */
    bool executeActions(DeviceManager& deviceManager);
    
    /**
     * @brief 获取规则ID
     * @return 规则ID
     */
    String getId();
    
    /**
     * @brief 设置规则ID
     * @param id 规则ID
     */
    void setId(String id);
    
    /**
     * @brief 获取规则名称
     * @return 规则名称
     */
    String getName();
    
    /**
     * @brief 检查规则是否启用
     * @return 启用返回true，否则返回false
     */
    bool isEnabled();
    
    /**
     * @brief 设置规则是否启用
     * @param enabled 启用状态
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief 获取规则条件列表
     * @return 规则条件列表的引用
     */
    const std::vector<RuleCondition>& getConditions() const;
    
    /**
     * @brief 获取规则动作列表
     * @return 规则动作列表的引用
     */
    const std::vector<RuleAction>& getActions() const;
    
    /**
     * @brief 设置规则名称
     * @param name 规则名称
     */
    void setName(String name);
    
    /**
     * @brief 清空规则条件列表
     */
    void clearConditions();
    
    /**
     * @brief 清空规则动作列表
     */
    void clearActions();
    
private:
    String id;                     // 规则ID
    String name;                   // 规则名称
    bool enabled;                  // 规则是否启用
    std::vector<RuleCondition> conditions;  // 规则条件列表
    std::vector<RuleAction> actions;        // 规则动作列表
};

#endif // RULE_H