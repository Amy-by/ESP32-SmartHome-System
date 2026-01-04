# ESP32智能家居系统数据库集成文档

## 1. 数据库方案选择

### 1.1 方案对比

| 方案 | 优点 | 缺点 | 适用场景 |
|------|------|------|----------|
| 方案一（本地存储） | 无需外部依赖，成本低 | 存储容量有限，数据易丢失，不便于多设备访问 | 小型、单设备、数据不敏感系统 |
| 方案二（MySQL数据库） | 存储容量大，数据安全性高，便于多设备访问，支持复杂查询和分析 | 需要额外的服务器资源，网络依赖 | 中型、多设备、需要数据分析的系统 |

### 1.2 最终选择

**选择方案二（MySQL数据库）**，原因如下：
- 保护ESP32资源：将数据存储和处理转移到外部服务器，减少ESP32的资源占用
- 数据持久化：避免因设备重启或断电导致的数据丢失
- 多设备访问：支持通过网页或其他设备远程访问和管理数据
- 数据分析：便于对历史数据进行查询和分析
- 可扩展性：便于系统功能扩展和数据量增长

## 2. 数据库设计

### 2.1 数据库结构

数据库名称：`esp32_smarthome`

#### 2.1.1 传感器类型表 (`sensor_types`)

| 字段名 | 数据类型 | 约束 | 描述 |
|--------|----------|------|------|
| `id` | INT | PRIMARY KEY, AUTO_INCREMENT | 传感器类型ID |
| `type` | VARCHAR(20) | NOT NULL, UNIQUE | 传感器类型标识符 |
| `name` | VARCHAR(50) | NOT NULL | 传感器类型名称 |
| `unit` | VARCHAR(10) | NOT NULL | 传感器测量单位 |

#### 2.1.2 设备表 (`devices`)

| 字段名 | 数据类型 | 约束 | 描述 |
|--------|----------|------|------|
| `id` | INT | PRIMARY KEY, AUTO_INCREMENT | 设备ID |
| `name` | VARCHAR(50) | NOT NULL | 设备名称 |
| `type` | VARCHAR(20) | NOT NULL | 设备类型 |
| `pin` | INT | NOT NULL, UNIQUE | 设备连接的GPIO引脚 |
| `status` | BOOLEAN | DEFAULT 0 | 设备状态（0：关闭，1：开启） |
| `brightness` | INT | DEFAULT 0 | 亮度值（仅适用于灯光设备，0-100） |
| `created_at` | TIMESTAMP | DEFAULT CURRENT_TIMESTAMP | 设备创建设时间 |

#### 2.1.3 传感器读数表 (`sensor_readings`)

| 字段名 | 数据类型 | 约束 | 描述 |
|--------|----------|------|------|
| `id` | INT | PRIMARY KEY, AUTO_INCREMENT | 读数ID |
| `sensor_id` | VARCHAR(20) | NOT NULL | 传感器ID |
| `type` | VARCHAR(20) | NOT NULL | 读数类型 |
| `value` | FLOAT | NOT NULL | 读数数值 |
| `timestamp` | TIMESTAMP | DEFAULT CURRENT_TIMESTAMP | 读数时间戳 |

**索引**：
- `idx_sensor_readings_sensor_timestamp`：加速按传感器ID和时间戳的查询
- `idx_sensor_readings_type`：加速按读数类型的查询

#### 2.1.4 规则表 (`rules`)

| 字段名 | 数据类型 | 约束 | 描述 |
|--------|----------|------|------|
| `id` | INT | PRIMARY KEY, AUTO_INCREMENT | 规则ID |
| `name` | VARCHAR(50) | NOT NULL | 规则名称 |
| `enabled` | TINYINT(1) | DEFAULT 1 | 规则是否启用（0：禁用，1：启用） |
| `created_at` | TIMESTAMP | DEFAULT CURRENT_TIMESTAMP | 规则创建设时间 |

**索引**：
- `idx_rules_enabled`：加速按启用状态的查询

#### 2.1.5 规则条件表 (`rule_conditions`)

| 字段名 | 数据类型 | 约束 | 描述 |
|--------|----------|------|------|
| `id` | INT | PRIMARY KEY, AUTO_INCREMENT | 条件ID |
| `rule_id` | INT | NOT NULL | 所属规则ID |
| `sensor_id` | VARCHAR(20) | NOT NULL | 传感器ID |
| `operator` | VARCHAR(10) | NOT NULL | 比较操作符（>、<、=、>=、<=） |
| `threshold` | FLOAT | NOT NULL | 阈值 |

**索引**：
- `idx_rule_conditions_rule_id`：加速按规则ID的查询

**外键**：
- `rule_id`：引用`rules(id)`，级联删除

#### 2.1.6 规则动作表 (`rule_actions`)

| 字段名 | 数据类型 | 约束 | 描述 |
|--------|----------|------|------|
| `id` | INT | PRIMARY KEY, AUTO_INCREMENT | 动作ID |
| `rule_id` | INT | NOT NULL | 所属规则ID |
| `device_id` | INT | NOT NULL | 目标设备ID |
| `target_status` | TINYINT(1) | NOT NULL | 目标状态（0：关闭，1：开启） |

**索引**：
- `idx_rule_actions_rule_id`：加速按规则ID的查询
- `idx_rule_actions_device_id`：加速按设备ID的查询

**外键**：
- `rule_id`：引用`rules(id)`，级联删除
- `device_id`：引用`devices(id)`，级联删除

#### 2.1.7 告警阈值表 (`alarm_thresholds`)

| 字段名 | 数据类型 | 约束 | 描述 |
|--------|----------|------|------|
| `id` | INT | PRIMARY KEY, AUTO_INCREMENT | 阈值ID |
| `sensor_id` | VARCHAR(20) | NOT NULL | 传感器ID |
| `type` | VARCHAR(20) | NOT NULL | 读数类型 |
| `min_threshold` | FLOAT | DEFAULT -1 | 最小阈值（-1表示无限制） |
| `max_threshold` | FLOAT | DEFAULT -1 | 最大阈值（-1表示无限制） |
| `enabled` | TINYINT(1) | DEFAULT 1 | 是否启用（0：禁用，1：启用） |

**索引**：
- `idx_alarm_thresholds_sensor_id`：加速按传感器ID的查询
- `idx_alarm_thresholds_enabled`：加速按启用状态的查询

#### 2.1.8 告警日志表 (`alarm_logs`)

| 字段名 | 数据类型 | 约束 | 描述 |
|--------|----------|------|------|
| `id` | INT | PRIMARY KEY, AUTO_INCREMENT | 日志ID |
| `sensor_id` | VARCHAR(20) | NOT NULL | 传感器ID |
| `type` | VARCHAR(20) | NOT NULL | 读数类型 |
| `value` | FLOAT | NOT NULL | 触发值 |
| `message` | VARCHAR(100) | NOT NULL | 告警消息 |
| `status` | VARCHAR(20) | DEFAULT 'active' | 告警状态（active：活动，resolved：已解决） |
| `timestamp` | TIMESTAMP | DEFAULT CURRENT_TIMESTAMP | 告警时间 |

**索引**：
- `idx_alarm_logs_sensor_id`：加速按传感器ID的查询

## 3. 代码实现

### 3.1 新增文件

#### 3.1.1 `src/utils/DatabaseManager.h`

定义数据库操作接口，包括：
- 数据库连接管理
- CRUD操作
- 传感器数据保存和查询
- 设备状态更新和查询
- 告警阈值管理

#### 3.1.2 `src/utils/DatabaseManager.cpp`

实现数据库操作功能，包括：
- 连接管理（自动重连机制）
- SQL查询执行
- 结果集处理
- 事务管理

### 3.2 修改文件

#### 3.2.1 `src/web/WebServer.h`

- 新增数据库管理器引用
- 更新构造函数参数

#### 3.2.2 `src/web/WebServer.cpp`

- 集成数据库功能，将数据获取和更新操作从本地存储改为数据库操作
- 实现API接口的数据获取和更新功能

#### 3.2.3 `platformio.ini`

- 新增MySQL连接库依赖：`chuckbell/MySQL Connector Arduino@^1.2.0`

### 3.3 核心功能实现

#### 3.3.1 数据库连接管理

```cpp
// 连接数据库
bool DatabaseManager::connect() {
    if (_connected) {
        return true;
    }
    
    unsigned long currentTime = millis();
    if (currentTime - _lastConnectionAttempt < _reconnectInterval) {
        return false;
    }
    
    _lastConnectionAttempt = currentTime;
    
    Serial.print("尝试连接到MySQL数据库...");
    
    // 连接到MySQL服务器
    if (_connection.connect(_serverAddress.c_str(), _serverPort, _client, _username.c_str(), _password.c_str())) {
        Serial.println("成功");
        
        // 选择数据库
        String useDbQuery = "USE " + _databaseName;
        if (executeQuery(useDbQuery)) {
            _connected = true;
            return true;
        } else {
            Serial.println("选择数据库失败");
            _connection.disconnect();
            return false;
        }
    } else {
        Serial.print("失败: ");
        Serial.println(_connection.connectError());
        return false;
    }
}
```

#### 3.3.2 传感器数据保存

```cpp
bool DatabaseManager::saveSensorData(const String& sensorId, const String& type, float value) {
    String query = "INSERT INTO sensor_readings (sensor_id, type, value) VALUES ('" + 
                   sensorId + "', '" + type + "', " + String(value) + ")";
    
    return executeQuery(query);
}
```

#### 3.3.3 设备控制

```cpp
void WebServer::handleControlDevice(AsyncWebServerRequest* request, const JsonVariantConst& json) {
    // ... 设备控制逻辑 ...
    
    if (result) {
        // 更新数据库中的设备状态
        int dbDeviceId = deviceId.toInt();
        if (hasStatus) {
            databaseManager.updateDeviceStatus(dbDeviceId, status);
        }
        if (hasBrightness && device->getType() == "light") {
            databaseManager.updateDeviceBrightness(dbDeviceId, brightness);
        }
        
        broadcastDeviceUpdate(device);
        DynamicJsonDocument doc(256);
        doc["status"] = "success";
        doc["message"] = "设备控制成功";
        sendJsonResponse(request, doc);
    } else {
        request->send(500, "application/json; charset=utf-8",
                      "{\"status\":\"error\",\"message\":\"设备控制失败\"}");
    }
}
```

#### 3.3.4 环境数据获取

```cpp
void WebServer::handleGetEnvironmentData(AsyncWebServerRequest* request) {
    DynamicJsonDocument doc(4096);
    JsonObject environment = doc.createNestedObject("environment");
    
    // 获取实时数据
    // ...
    
    // 添加历史环境数据
    JsonObject historical = doc.createNestedObject("historical");
    
    // 获取温度历史数据
    String tempHistory;
    if (databaseManager.getSensorReadings("DHT22", 10, tempHistory)) {
        DynamicJsonDocument tempDoc(2048);
        deserializeJson(tempDoc, tempHistory);
        historical["temperature"] = tempDoc["rows"];
    }
    
    // 获取湿度历史数据
    String humidityHistory;
    if (databaseManager.getSensorReadings("DHT22", 10, humidityHistory)) {
        DynamicJsonDocument humidityDoc(2048);
        deserializeJson(humidityDoc, humidityHistory);
        historical["humidity"] = humidityDoc["rows"];
    }
    
    // 获取光照历史数据
    String lightHistory;
    if (databaseManager.getSensorReadings("BH1750", 10, lightHistory)) {
        DynamicJsonDocument lightDoc(2048);
        deserializeJson(lightDoc, lightHistory);
        historical["light"] = lightDoc["rows"];
    }
    
    // 获取烟雾历史数据
    String smokeHistory;
    if (databaseManager.getSensorReadings("MQ2", 10, smokeHistory)) {
        DynamicJsonDocument smokeDoc(2048);
        deserializeJson(smokeDoc, smokeHistory);
        historical["smoke"] = smokeDoc["rows"];
    }
    
    doc["status"] = "success";
    doc["message"] = "获取环境数据成功";
    
    sendJsonResponse(request, doc);
}
```

## 4. 性能优化

### 4.1 索引优化

为以下查询场景创建了索引：

1. **传感器数据查询**：按传感器ID和时间戳查询
   ```sql
   INDEX idx_sensor_readings_sensor_timestamp (sensor_id, timestamp)
   ```

2. **传感器类型查询**：按读数类型查询
   ```sql
   INDEX idx_sensor_readings_type (type)
   ```

3. **规则查询**：按规则启用状态查询
   ```sql
   INDEX idx_rules_enabled (enabled)
   ```

4. **规则条件查询**：按规则ID查询
   ```sql
   INDEX idx_rule_conditions_rule_id (rule_id)
   ```

5. **规则动作查询**：按规则ID和设备ID查询
   ```sql
   INDEX idx_rule_actions_rule_id (rule_id)
   INDEX idx_rule_actions_device_id (device_id)
   ```

6. **告警阈值查询**：按传感器ID和启用状态查询
   ```sql
   INDEX idx_alarm_thresholds_sensor_id (sensor_id)
   INDEX idx_alarm_thresholds_enabled (enabled)
   ```

7. **告警日志查询**：按传感器ID查询
   ```sql
   INDEX idx_alarm_logs_sensor_id (sensor_id)
   ```

### 4.2 定期清理

#### 4.2.1 传感器数据清理

创建事件调度器，定期清理30天前的传感器数据：

```sql
CREATE EVENT IF NOT EXISTS cleanup_old_sensor_readings
ON SCHEDULE EVERY 1 DAY
STARTS CURRENT_DATE + INTERVAL 1 DAY
ON COMPLETION PRESERVE
ENABLE
DO
DELETE FROM sensor_readings WHERE timestamp < DATE_SUB(NOW(), INTERVAL 30 DAY);
```

#### 4.2.2 告警日志清理

创建事件调度器，定期清理90天前的告警日志：

```sql
CREATE EVENT IF NOT EXISTS cleanup_old_alarm_logs
ON SCHEDULE EVERY 1 DAY
STARTS CURRENT_DATE + INTERVAL 1 DAY
ON COMPLETION PRESERVE
ENABLE
DO
DELETE FROM alarm_logs WHERE timestamp < DATE_SUB(NOW(), INTERVAL 90 DAY);
```

## 5. 网页界面集成

### 5.1 数据获取

- **设备列表**：从数据库获取所有设备信息
- **设备详情**：从数据库获取单个设备详细信息
- **环境数据**：从数据库获取实时和历史环境数据
- **告警阈值**：从数据库获取告警阈值设置

### 5.2 数据更新

- **设备控制**：更新设备状态和亮度，并保存到数据库
- **告警阈值设置**：更新告警阈值，并保存到数据库

### 5.3 API接口

| API路径 | 方法 | 功能 | 数据来源 |
|---------|------|------|----------|
| `/api/devices` | GET | 获取所有设备 | 数据库 |
| `/api/device` | GET | 获取单个设备 | 数据库 |
| `/api/devices/control` | POST | 控制设备 | 数据库 |
| `/api/environment` | GET | 获取环境数据 | 数据库 |
| `/api/alarm` | GET | 获取告警阈值 | 数据库 |
| `/api/alarm` | PUT/POST | 更新告警阈值 | 数据库 |

## 6. 部署和配置

### 6.1 数据库部署

使用提供的`database/schema.sql`脚本进行数据库初始化：

```bash
mysql -u root -p < database/schema.sql
```

### 6.2 MySQL配置

确保MySQL服务器已启用事件调度器：

```sql
-- 查看事件调度器状态
SHOW VARIABLES LIKE 'event_scheduler';

-- 启用事件调度器
SET GLOBAL event_scheduler = ON;
```

### 6.3 ESP32配置

在`config.json`中配置数据库连接信息：

```json
{
  "database": {
    "server": "192.168.1.100",
    "port": 3306,
    "username": "esp32_user",
    "password": "esp32_password",
    "name": "esp32_smarthome"
  }
}
```

## 7. 故障排除

### 7.1 连接问题

- 确保MySQL服务器已启动并运行
- 检查网络连接和防火墙设置
- 验证数据库用户名和密码
- 确保MySQL用户具有远程访问权限

### 7.2 查询问题

- 检查SQL语法是否正确
- 确保表和字段名称正确
- 验证数据库用户权限

### 7.3 性能问题

- 检查索引使用情况
- 确保定期清理任务正常运行
- 优化查询语句
- 考虑增加硬件资源

## 8. 总结

本项目成功实现了ESP32智能家居系统与MySQL数据库的集成，具有以下特点：

1. **数据持久化**：确保数据不丢失，便于长期保存和分析
2. **资源优化**：将数据处理和存储转移到外部服务器，减少ESP32资源占用
3. **多设备访问**：支持通过网页或其他设备远程访问和管理数据
4. **性能优化**：通过索引和定期清理提高系统性能
5. **可扩展性**：便于系统功能扩展和数据量增长

数据库集成显著提升了系统的稳定性、可靠性和可扩展性，为用户提供了更好的使用体验和数据管理能力。