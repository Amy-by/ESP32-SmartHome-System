#include "DatabaseManager.h"
#include <ArduinoJson.h>

DatabaseManager::DatabaseManager(ConfigManager& configManager)
    : _client(),
      _connection(&_client),
      _configManager(configManager),
      _connected(false),
      _lastConnectionAttempt(0),
      _reconnectInterval(5000) // 5秒重连间隔
{
    // 从配置中获取数据库连接信息
    JsonDocument& config = _configManager.getFullConfig();
    
    // 设置默认值
    _serverAddress = config["database"]["server"].as<String>() || "192.168.1.100";
    _serverPort = config["database"]["port"].as<int>() || 3306;
    _username = config["database"]["username"].as<String>() || "esp32_user";
    _password = config["database"]["password"].as<String>() || "esp32_password";
    _databaseName = config["database"]["name"].as<String>() || "esp32_smarthome";
}

DatabaseManager::~DatabaseManager()
{
    disconnect();
}

bool DatabaseManager::init()
{
    // 尝试连接数据库
    return connect();
}

bool DatabaseManager::connect()
{
    if (_connected) {
        return true;
    }
    
    unsigned long currentTime = millis();
    if (currentTime - _lastConnectionAttempt < _reconnectInterval) {
        return false;
    }
    
    _lastConnectionAttempt = currentTime;
    
    Serial.print("尝试连接到MySQL数据库...");
    
    // 将服务器地址转换为IPAddress
    IPAddress serverIp;
    if (!serverIp.fromString(_serverAddress)) {
        Serial.println("无效的服务器地址");
        return false;
    }
    
    // 连接到MySQL服务器
    if (_connection.connect(serverIp, _serverPort, const_cast<char*>(_username.c_str()), const_cast<char*>(_password.c_str()))) {
        Serial.println("成功");
        
        // 选择数据库
        String useDbQuery = "USE " + _databaseName;
        if (executeQuery(useDbQuery)) {
            _connected = true;
            return true;
        } else {
            Serial.println("选择数据库失败");
            return false;
        }
    } else {
        Serial.println("连接失败");
        return false;
    }
}

void DatabaseManager::disconnect()
{
    if (_connected) {
        // MySQL_Connection没有disconnect()方法，直接关闭客户端连接
        _client.stop();
        _connected = false;
        Serial.println("已断开MySQL数据库连接");
    }
}

bool DatabaseManager::isConnected()
{
    if (_connected) {
        // 检查连接是否仍然有效
        _connected = _connection.connected();
    }
    return _connected;
}

bool DatabaseManager::executeQuery(const String& query)
{
    if (!connect()) {
        return false;
    }
    
    MySQL_Cursor cursor(&_connection);
    
    // 执行查询
    if (cursor.execute(query.c_str())) {
        cursor.close();
        return true;
    } else {
        Serial.print("SQL查询执行失败: ");
        Serial.println(query);
        cursor.close();
        _connected = false; // 标记连接为断开
        return false;
    }
}

bool DatabaseManager::executeSelectQuery(const String& query, String& result)
{
    if (!connect()) {
        return false;
    }
    
    MySQL_Cursor cursor(&_connection);
    
    // 执行查询
    if (cursor.execute(query.c_str())) {
        row_values *row = NULL;
        
        // 构造JSON结果
        DynamicJsonDocument doc(1024);
        JsonArray rows = doc.createNestedArray("rows");
        
        // 获取列名
        column_names *cols = cursor.get_columns();
        
        // 遍历结果集
        while ((row = cursor.get_next_row()) != NULL) {
            JsonObject jsonRow = rows.createNestedObject();
            for (int i = 0; i < cols->num_fields; i++) {
                jsonRow[cols->fields[i]->name] = row->values[i] ? row->values[i] : "NULL";
            }
        }
        
        // 序列化为字符串
        serializeJson(doc, result);
        
        cursor.close();
        return true;
    } else {
        Serial.print("SQL查询执行失败: ");
        Serial.println(query);
        cursor.close();
        _connected = false; // 标记连接为断开
        return false;
    }
}

bool DatabaseManager::saveSensorData(const String& sensorId, const String& type, float value)
{
    String query = "INSERT INTO sensor_readings (sensor_id, type, value) VALUES ('" + 
                   sensorId + "', '" + type + "', " + String(value) + ")";
    
    return executeQuery(query);
}

bool DatabaseManager::updateDeviceStatus(int deviceId, bool status)
{
    String query = "UPDATE devices SET status = " + String(status) + 
                   " WHERE id = " + String(deviceId);
    
    return executeQuery(query);
}

int DatabaseManager::getDeviceStatus(int deviceId)
{
    String query = "SELECT status FROM devices WHERE id = " + String(deviceId);
    String result;
    
    if (executeSelectQuery(query, result)) {
        DynamicJsonDocument doc(512);
        deserializeJson(doc, result);
        
        if (doc["rows"].size() > 0) {
            return doc["rows"][0]["status"].as<int>();
        }
    }
    
    return -1; // 失败
}

void DatabaseManager::maintenance()
{
    if (!isConnected()) {
        connect();
    }
    
    // 定期清理旧的传感器数据（每天执行一次）
    static unsigned long lastCleanupTime = 0;
    static const unsigned long CLEANUP_INTERVAL = 24 * 60 * 60 * 1000; // 24小时
    static const int RETAIN_DAYS = 7; // 保留最近7天的数据
    
    if (millis() - lastCleanupTime >= CLEANUP_INTERVAL) {
        lastCleanupTime = millis();
        
        if (isConnected()) {
            Serial.println("执行数据库清理：删除7天前的传感器数据");
            if (deleteOldSensorReadings(RETAIN_DAYS)) {
                Serial.println("数据库清理完成");
            } else {
                Serial.println("数据库清理失败");
            }
        }
    }
}

void DatabaseManager::setServerAddress(const String& address)
{
    _serverAddress = address;
}

void DatabaseManager::setServerPort(int port)
{
    _serverPort = port;
}

void DatabaseManager::setUsername(const String& username)
{
    _username = username;
}

void DatabaseManager::setPassword(const String& password)
{
    _password = password;
}

void DatabaseManager::setDatabaseName(const String& databaseName)
{
    _databaseName = databaseName;
}

bool DatabaseManager::getAllDevices(String& result)
{
    String query = "SELECT * FROM devices";
    return executeSelectQuery(query, result);
}

bool DatabaseManager::createDevice(const String& name, const String& type, int pin, bool status, int brightness)
{
    String query = "INSERT INTO devices (name, type, pin, status, brightness) VALUES ('" + 
                   name + "', '" + type + "', " + String(pin) + ", " + 
                   String(status ? 1 : 0) + ", " + String(brightness) + ")";
    
    return executeQuery(query);
}

bool DatabaseManager::deleteDevice(int deviceId)
{
    String query = "DELETE FROM devices WHERE id = " + String(deviceId);
    return executeQuery(query);
}

bool DatabaseManager::updateDeviceBrightness(int deviceId, int brightness)
{
    String query = "UPDATE devices SET brightness = " + String(brightness) + 
                   " WHERE id = " + String(deviceId);
    return executeQuery(query);
}

bool DatabaseManager::getSensorReadings(const String& sensorId, int limit, String& result)
{
    String query = "SELECT * FROM sensor_readings WHERE sensor_id = '" + 
                   sensorId + "' ORDER BY timestamp DESC LIMIT " + String(limit);
    return executeSelectQuery(query, result);
}

bool DatabaseManager::getAllSensorTypes(String& result)
{
    String query = "SELECT * FROM sensor_types";
    return executeSelectQuery(query, result);
}

bool DatabaseManager::getAlarmThresholds(const String& sensorId, String& result)
{
    String query = "SELECT * FROM alarm_thresholds WHERE sensor_id = '" + sensorId + "'";
    return executeSelectQuery(query, result);
}

bool DatabaseManager::updateAlarmThreshold(const String& sensorId, const String& type, float minThreshold, float maxThreshold, bool enabled)
{
    // 检查是否已存在记录
    String checkQuery = "SELECT id FROM alarm_thresholds WHERE sensor_id = '" + sensorId + "' AND type = '" + type + "'";
    String checkResult;
    
    if (executeSelectQuery(checkQuery, checkResult)) {
        DynamicJsonDocument doc(512);
        deserializeJson(doc, checkResult);
        
        if (doc["rows"].size() > 0) {
            // 更新现有记录
            String query = "UPDATE alarm_thresholds SET min_threshold = " + String(minThreshold) + 
                           ", max_threshold = " + String(maxThreshold) + 
                           ", enabled = " + String(enabled ? 1 : 0) + 
                           " WHERE sensor_id = '" + sensorId + "' AND type = '" + type + "'";
            return executeQuery(query);
        } else {
            // 插入新记录
            String query = "INSERT INTO alarm_thresholds (sensor_id, type, min_threshold, max_threshold, enabled) VALUES ('" + 
                           sensorId + "', '" + type + "', " + String(minThreshold) + ", " + 
                           String(maxThreshold) + ", " + String(enabled ? 1 : 0) + ")";
            return executeQuery(query);
        }
    }
    
    return false;
}

bool DatabaseManager::getAllRules(String& result)
{
    String query = "SELECT r.id, r.name, r.enabled, "
                   "GROUP_CONCAT(CONCAT('[条件] ', rc.sensor_id, ' ', rc.operator, ' ', rc.threshold) SEPARATOR '\n') AS conditions, "
                   "GROUP_CONCAT(CONCAT('[动作] ', d.name, ' -> ', ra.target_status) SEPARATOR '\n') AS actions "
                   "FROM rules r "
                   "LEFT JOIN rule_conditions rc ON r.id = rc.rule_id "
                   "LEFT JOIN rule_actions ra ON r.id = ra.rule_id "
                   "LEFT JOIN devices d ON ra.device_id = d.id "
                   "GROUP BY r.id";
    
    return executeSelectQuery(query, result);
}

bool DatabaseManager::updateRuleStatus(int ruleId, bool enabled)
{
    String query = "UPDATE rules SET enabled = " + String(enabled ? 1 : 0) + 
                   " WHERE id = " + String(ruleId);
    return executeQuery(query);
}

bool DatabaseManager::deleteOldSensorReadings(int days)
{
    String query = "DELETE FROM sensor_readings WHERE timestamp < DATE_SUB(NOW(), INTERVAL " + 
                   String(days) + " DAY)";
    return executeQuery(query);
}

int DatabaseManager::createRule(const String& name, bool enabled, const std::vector<RuleCondition>& conditions, const std::vector<RuleAction>& actions)
{
    if (!isConnected()) {
        connect();
        if (!isConnected()) {
            return -1;
        }
    }

    // 创建规则主记录
    String query = "INSERT INTO rules (name, enabled) VALUES ('" + escapeString(name) + "', " + String(enabled ? 1 : 0) + ")";
    if (!executeQuery(query)) {
        return -1;
    }

    // 获取新创建的规则ID
    query = "SELECT LAST_INSERT_ID() as rule_id";
    String result;
    if (!executeSelectQuery(query, result)) {
        return -1;
    }

    // 解析规则ID
    int ruleId = -1;
    DynamicJsonDocument doc(512);
    if (deserializeJson(doc, result) == DeserializationError::Ok) {
        if (doc["rows"].size() > 0) {
            ruleId = doc["rows"][0]["rule_id"].as<int>();
        }
    }

    if (ruleId == -1) {
        return -1;
    }

    // 插入规则条件
    for (const RuleCondition& condition : conditions) {
        query = "INSERT INTO rule_conditions (rule_id, sensor_id, operator, threshold) VALUES (" + 
                String(ruleId) + ", '" + escapeString(condition.sensorId) + "', '" + escapeString(condition.operatorType) + "', " + 
                String(condition.threshold) + ")";
        executeQuery(query);
    }

    // 插入规则动作
    for (const RuleAction& action : actions) {
        // 解析设备ID（假设设备ID是数字形式的字符串）
        int deviceId = action.deviceId.toInt();
        query = "INSERT INTO rule_actions (rule_id, device_id, target_status) VALUES (" + 
                String(ruleId) + ", " + String(deviceId) + ", " + String(action.targetStatus ? 1 : 0) + ")";
        executeQuery(query);
    }

    return ruleId;
}

bool DatabaseManager::updateRule(int ruleId, const String& name, bool enabled, const std::vector<RuleCondition>& conditions, const std::vector<RuleAction>& actions)
{
    if (!isConnected()) {
        connect();
        if (!isConnected()) {
            return false;
        }
    }

    // 更新规则主信息
    String query = "UPDATE rules SET name = '" + escapeString(name) + "', enabled = " + String(enabled ? 1 : 0) + " WHERE id = " + String(ruleId);
    if (!executeQuery(query)) {
        return false;
    }

    // 删除旧的条件和动作
    query = "DELETE FROM rule_conditions WHERE rule_id = " + String(ruleId);
    executeQuery(query);
    
    query = "DELETE FROM rule_actions WHERE rule_id = " + String(ruleId);
    executeQuery(query);

    // 插入新的条件
    for (const RuleCondition& condition : conditions) {
        query = "INSERT INTO rule_conditions (rule_id, sensor_id, operator, threshold) VALUES (" + 
                String(ruleId) + ", '" + escapeString(condition.sensorId) + "', '" + escapeString(condition.operatorType) + "', " + 
                String(condition.threshold) + ")";
        executeQuery(query);
    }

    // 插入新的动作
    for (const RuleAction& action : actions) {
        // 解析设备ID（假设设备ID是数字形式的字符串）
        int deviceId = action.deviceId.toInt();
        query = "INSERT INTO rule_actions (rule_id, device_id, target_status) VALUES (" + 
                String(ruleId) + ", " + String(deviceId) + ", " + String(action.targetStatus ? 1 : 0) + ")";
        executeQuery(query);
    }

    return true;
}

bool DatabaseManager::deleteRule(int ruleId)
{
    if (!isConnected()) {
        connect();
        if (!isConnected()) {
            return false;
        }
    }

    // 删除规则（级联删除条件和动作）
    String query = "DELETE FROM rules WHERE id = " + String(ruleId);
    return executeQuery(query);
}

bool DatabaseManager::getRuleById(int ruleId, String& result)
{
    if (!isConnected()) {
        connect();
        if (!isConnected()) {
            return false;
        }
    }

    String query = "SELECT r.id, r.name, r.enabled, "
                   "GROUP_CONCAT(CONCAT('[条件] ', rc.sensor_id, ' ', rc.operator, ' ', rc.threshold) SEPARATOR '\\n') AS conditions, "
                   "GROUP_CONCAT(CONCAT('[动作] ', d.name, ' -> ', ra.target_status) SEPARATOR '\\n') AS actions "
                   "FROM rules r "
                   "LEFT JOIN rule_conditions rc ON r.id = rc.rule_id "
                   "LEFT JOIN rule_actions ra ON r.id = ra.rule_id "
                   "LEFT JOIN devices d ON ra.device_id = d.id "
                   "WHERE r.id = " + String(ruleId) + " "
                   "GROUP BY r.id";

    return executeSelectQuery(query, result);
}

String DatabaseManager::escapeString(const String& str)
{
    String escaped = str;
    escaped.replace("'", "''");
    escaped.replace("\\", "\\\\");
    return escaped;
}