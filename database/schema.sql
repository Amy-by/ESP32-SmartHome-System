-- ESP32智能家居系统数据库初始化脚本
-- 创建数据库
CREATE DATABASE IF NOT EXISTS esp32_smarthome CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- 使用数据库
USE esp32_smarthome;

-- 创建用户并授权（可选，根据需要取消注释）
-- CREATE USER IF NOT EXISTS 'esp32_user'@'%' IDENTIFIED BY 'esp32_password';
-- GRANT ALL PRIVILEGES ON esp32_smarthome.* TO 'esp32_user'@'%';
-- FLUSH PRIVILEGES;

-- 创建传感器类型表
CREATE TABLE IF NOT EXISTS sensor_types (
    id INT AUTO_INCREMENT PRIMARY KEY,
    type VARCHAR(20) NOT NULL UNIQUE,
    name VARCHAR(50) NOT NULL,
    unit VARCHAR(10) NOT NULL
) ENGINE=InnoDB;

-- 创建设备表
CREATE TABLE IF NOT EXISTS devices (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
    type VARCHAR(20) NOT NULL,
    pin INT NOT NULL,
    status BOOLEAN DEFAULT 0,
    brightness INT DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE KEY unique_pin (pin)
) ENGINE=InnoDB;

-- 创建传感器读数表
CREATE TABLE IF NOT EXISTS sensor_readings (
    id INT AUTO_INCREMENT PRIMARY KEY,
    sensor_id VARCHAR(20) NOT NULL,
    type VARCHAR(20) NOT NULL,
    value FLOAT NOT NULL,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_sensor_readings_sensor_timestamp (sensor_id, timestamp),
    INDEX idx_sensor_readings_type (type)
) ENGINE=InnoDB;

-- 创建规则表
CREATE TABLE IF NOT EXISTS rules (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(50) NOT NULL,
    enabled TINYINT(1) DEFAULT 1,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_rules_enabled (enabled)
) ENGINE=InnoDB;

-- 创建规则条件表
CREATE TABLE IF NOT EXISTS rule_conditions (
    id INT AUTO_INCREMENT PRIMARY KEY,
    rule_id INT NOT NULL,
    sensor_id VARCHAR(20) NOT NULL,
    operator VARCHAR(10) NOT NULL,
    threshold FLOAT NOT NULL,
    FOREIGN KEY (rule_id) REFERENCES rules(id) ON DELETE CASCADE,
    INDEX idx_rule_conditions_rule_id (rule_id)
) ENGINE=InnoDB;

-- 创建规则动作表
CREATE TABLE IF NOT EXISTS rule_actions (
    id INT AUTO_INCREMENT PRIMARY KEY,
    rule_id INT NOT NULL,
    device_id INT NOT NULL,
    target_status TINYINT(1) NOT NULL,
    FOREIGN KEY (rule_id) REFERENCES rules(id) ON DELETE CASCADE,
    FOREIGN KEY (device_id) REFERENCES devices(id) ON DELETE CASCADE,
    INDEX idx_rule_actions_rule_id (rule_id),
    INDEX idx_rule_actions_device_id (device_id)
) ENGINE=InnoDB;

-- 创建告警阈值表
CREATE TABLE IF NOT EXISTS alarm_thresholds (
    id INT AUTO_INCREMENT PRIMARY KEY,
    sensor_id VARCHAR(20) NOT NULL,
    type VARCHAR(20) NOT NULL,
    min_threshold FLOAT DEFAULT -1,
    max_threshold FLOAT DEFAULT -1,
    enabled TINYINT(1) DEFAULT 1,
    INDEX idx_alarm_thresholds_sensor_id (sensor_id),
    INDEX idx_alarm_thresholds_enabled (enabled)
) ENGINE=InnoDB;

-- 创建告警日志表
CREATE TABLE IF NOT EXISTS alarm_logs (
    id INT AUTO_INCREMENT PRIMARY KEY,
    sensor_id VARCHAR(20) NOT NULL,
    type VARCHAR(20) NOT NULL,
    value FLOAT NOT NULL,
    message VARCHAR(100) NOT NULL,
    status VARCHAR(20) DEFAULT 'active',
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_alarm_logs_sensor_id (sensor_id)
) ENGINE=InnoDB;

-- 插入初始数据

-- 传感器类型数据
INSERT INTO sensor_types (type, name, unit) VALUES
('temperature', '温度传感器', '°C'),
('humidity', '湿度传感器', '%'),
('light', '光照传感器', 'lux'),
('motion', '人体感应', '状态'),
('smoke', '烟雾传感器', 'ppm');

-- 设备数据
INSERT INTO devices (name, type, pin, status, brightness) VALUES
('智能灯', 'light', 2, 0, 0),
('智能开关', 'switch', 12, 0, 0);

-- 传感器阈值数据
INSERT INTO alarm_thresholds (sensor_id, type, min_threshold, max_threshold, enabled) VALUES
('DHT22', 'temperature', 18.0, 28.0, 1),
('DHT22', 'humidity', 40.0, 70.0, 1),
('LDR', 'light', 50.0, 1000.0, 1);

-- 规则数据
INSERT INTO rules (name, enabled) VALUES
('温度过高开风扇', 1),
('光线暗自动开灯', 1);

-- 规则条件数据
INSERT INTO rule_conditions (rule_id, sensor_id, operator, threshold) VALUES
(1, 'DHT22', '>', 26.0),
(2, 'LDR', '<', 100.0);

-- 规则动作数据
INSERT INTO rule_actions (rule_id, device_id, target_status) VALUES
(1, 2, 1),
(2, 1, 1);

-- 显示创建的表
SHOW TABLES;

-- 显示表数据统计
SELECT 'sensor_types' AS table_name, COUNT(*) AS records FROM sensor_types UNION
SELECT 'devices' AS table_name, COUNT(*) AS records FROM devices UNION
SELECT 'alarm_thresholds' AS table_name, COUNT(*) AS records FROM alarm_thresholds UNION
SELECT 'rules' AS table_name, COUNT(*) AS records FROM rules;

-- 创建事件调度器（用于定期清理旧数据）
-- 注意：需要确保MySQL的event_scheduler已启用
-- 查看状态：SHOW VARIABLES LIKE 'event_scheduler';
-- 启用：SET GLOBAL event_scheduler = ON;

-- 创建定期清理传感器读数的事件
CREATE EVENT IF NOT EXISTS cleanup_old_sensor_readings
ON SCHEDULE EVERY 1 DAY
STARTS CURRENT_DATE + INTERVAL 1 DAY
ON COMPLETION PRESERVE
ENABLE
DO
DELETE FROM sensor_readings WHERE timestamp < DATE_SUB(NOW(), INTERVAL 30 DAY);

-- 创建定期清理告警日志的事件
CREATE EVENT IF NOT EXISTS cleanup_old_alarm_logs
ON SCHEDULE EVERY 1 DAY
STARTS CURRENT_DATE + INTERVAL 1 DAY
ON COMPLETION PRESERVE
ENABLE
DO
DELETE FROM alarm_logs WHERE timestamp < DATE_SUB(NOW(), INTERVAL 90 DAY);

-- 数据库初始化完成
SELECT '数据库初始化完成' AS message;