#include "ConfigManager.h"

ConfigManager::ConfigManager(SPIFFSStorage& spiffsStorage, String configFilePath) 
    : _spiffsStorage(spiffsStorage), 
      _configFilePath(configFilePath), 
      _config(2048),
      _loaded(false)
{
    // 初始化默认配置
    resetToDefaults();
}

ConfigManager::~ConfigManager() {
    // 保存配置
    save();
}

bool ConfigManager::load() {
    if (_spiffsStorage.readJsonFile(_configFilePath, _config)) {
        _loaded = true;
        return true;
    }
    return false;
}

bool ConfigManager::save() {
    return _spiffsStorage.writeJsonFile(_configFilePath, _config);
}

void ConfigManager::resetToDefaults() {
    // WiFi配置
    _config["wifi"]["ssid"] = DEFAULT_WIFI_SSID;
    _config["wifi"]["password"] = DEFAULT_WIFI_PASSWORD;
    _config["wifi"]["dhcp_enabled"] = DEFAULT_WIFI_DHCP_ENABLED;
    
    // 静态IP配置
    JsonObject staticIp = _config["wifi"]["static_ip"].to<JsonObject>();
    staticIp["ip"] = DEFAULT_STATIC_IP.toString();
    staticIp["gateway"] = DEFAULT_GATEWAY.toString();
    staticIp["subnet_mask"] = DEFAULT_SUBNET_MASK.toString();
    staticIp["dns_server"] = DEFAULT_DNS_SERVER.toString();
    
    // 传感器配置
    JsonObject sensors = _config["sensors"].to<JsonObject>();
    sensors["update_interval"] = DEFAULT_SENSOR_UPDATE_INTERVAL;
    sensors["temperature_threshold"] = DEFAULT_TEMPERATURE_THRESHOLD;
    sensors["humidity_threshold"] = DEFAULT_HUMIDITY_THRESHOLD;
    sensors["light_threshold"] = DEFAULT_LIGHT_THRESHOLD;
    sensors["smoke_threshold"] = DEFAULT_SMOKE_THRESHOLD;
    
    // 设备配置
    JsonObject devices = _config["devices"].to<JsonObject>();
    devices["polling_interval"] = DEFAULT_DEVICE_POLLING_INTERVAL;
    
    // 存储配置
    JsonObject storage = _config["storage"].to<JsonObject>();
    storage["max_log_size"] = DEFAULT_MAX_LOG_SIZE;
    storage["spiffs_max_file_size"] = SPIFFS_MAX_FILE_SIZE;
    
    // Web服务器配置
    JsonObject web = _config["web"].to<JsonObject>();
    web["port"] = DEFAULT_WEB_SERVER_PORT;
    
    // 主循环配置
    JsonObject mainLoop = _config["main_loop"].to<JsonObject>();
    mainLoop["delay_ms"] = MAIN_LOOP_DELAY_MS;
    mainLoop["save_interval_ms"] = SAVE_INTERVAL_MS;
    
    // 告警配置
    JsonObject alarm = _config["alarm"].to<JsonObject>();
    alarm["duration"] = DEFAULT_ALARM_DURATION;
    alarm["enabled"] = DEFAULT_ALARM_ENABLED;
    
    // 调试配置
    JsonObject debug = _config["debug"].to<JsonObject>();
    debug["mode"] = DEFAULT_DEBUG_MODE;
    
    // 数据库配置
    JsonObject database = _config["database"].to<JsonObject>();
    database["server"] = "192.168.1.100";
    database["port"] = 3306;
    database["username"] = "esp32_user";
    database["password"] = "esp32_password";
    database["name"] = "esp32_smarthome";
    
    // 管理员账号配置（方案一）
    JsonObject admin = _config["admin"].to<JsonObject>();
    admin["username"] = "admin";
    admin["password"] = "admin123";
}

String ConfigManager::getWiFiSsid() {
    return _config["wifi"]["ssid"].as<String>();
}

void ConfigManager::setWiFiSsid(const String& ssid) {
    _config["wifi"]["ssid"] = ssid;
}

String ConfigManager::getWiFiPassword() {
    return _config["wifi"]["password"].as<String>();
}

void ConfigManager::setWiFiPassword(const String& password) {
    _config["wifi"]["password"] = password;
}

float ConfigManager::getTemperatureThreshold() {
    return _config["sensors"]["temperature_threshold"].as<float>();
}

void ConfigManager::setTemperatureThreshold(float threshold) {
    _config["sensors"]["temperature_threshold"] = threshold;
}

float ConfigManager::getHumidityThreshold() {
    return _config["sensors"]["humidity_threshold"].as<float>();
}

void ConfigManager::setHumidityThreshold(float threshold) {
    _config["sensors"]["humidity_threshold"] = threshold;
}

int ConfigManager::getLightThreshold() {
    return _config["sensors"]["light_threshold"].as<int>();
}

void ConfigManager::setLightThreshold(int threshold) {
    _config["sensors"]["light_threshold"] = threshold;
}

int ConfigManager::getSmokeThreshold() {
    return _config["sensors"]["smoke_threshold"].as<int>();
}

void ConfigManager::setSmokeThreshold(int threshold) {
    _config["sensors"]["smoke_threshold"] = threshold;
}

int ConfigManager::getMaxLogSize() {
    return _config["storage"]["max_log_size"].as<int>();
}

void ConfigManager::setMaxLogSize(int size) {
    _config["storage"]["max_log_size"] = size;
}

int ConfigManager::getMainLoopDelayMs() {
    return _config["main_loop"]["delay_ms"].as<int>();
}

void ConfigManager::setMainLoopDelayMs(int delay) {
    _config["main_loop"]["delay_ms"] = delay;
}

bool ConfigManager::isLoaded() const {
    return _loaded;
}

bool ConfigManager::getWiFiDhcpEnabled() {
    return _config["wifi"]["dhcp_enabled"].as<bool>();
}

void ConfigManager::setWiFiDhcpEnabled(bool enabled) {
    _config["wifi"]["dhcp_enabled"] = enabled;
}

String ConfigManager::getWiFiStaticIp() {
    return _config["wifi"]["static_ip"]["ip"].as<String>();
}

void ConfigManager::setWiFiStaticIp(const String& ip) {
    _config["wifi"]["static_ip"]["ip"] = ip;
}

String ConfigManager::getWiFiGateway() {
    return _config["wifi"]["static_ip"]["gateway"].as<String>();
}

void ConfigManager::setWiFiGateway(const String& gateway) {
    _config["wifi"]["static_ip"]["gateway"] = gateway;
}

String ConfigManager::getWiFiSubnetMask() {
    return _config["wifi"]["static_ip"]["subnet_mask"].as<String>();
}

void ConfigManager::setWiFiSubnetMask(const String& subnetMask) {
    _config["wifi"]["static_ip"]["subnet_mask"] = subnetMask;
}

String ConfigManager::getWiFiDnsServer() {
    return _config["wifi"]["static_ip"]["dns_server"].as<String>();
}

void ConfigManager::setWiFiDnsServer(const String& dnsServer) {
    _config["wifi"]["static_ip"]["dns_server"] = dnsServer;
}

int ConfigManager::getSensorUpdateInterval() {
    return _config["sensors"]["update_interval"].as<int>();
}

void ConfigManager::setSensorUpdateInterval(int interval) {
    _config["sensors"]["update_interval"] = interval;
}

int ConfigManager::getDevicePollingInterval() {
    return _config["devices"]["polling_interval"].as<int>();
}

void ConfigManager::setDevicePollingInterval(int interval) {
    _config["devices"]["polling_interval"] = interval;
}

int ConfigManager::getWebServerPort() {
    return _config["web"]["port"].as<int>();
}

void ConfigManager::setWebServerPort(int port) {
    _config["web"]["port"] = port;
}

int ConfigManager::getAlarmDuration() {
    return _config["alarm"]["duration"].as<int>();
}

void ConfigManager::setAlarmDuration(int duration) {
    _config["alarm"]["duration"] = duration;
}

bool ConfigManager::getAlarmEnabled() {
    return _config["alarm"]["enabled"].as<bool>();
}

void ConfigManager::setAlarmEnabled(bool enabled) {
    _config["alarm"]["enabled"] = enabled;
}

bool ConfigManager::getDebugMode() {
    return _config["debug"]["mode"].as<bool>();
}

void ConfigManager::setDebugMode(bool mode) {
    _config["debug"]["mode"] = mode;
}

JsonDocument& ConfigManager::getFullConfig() {
    return _config;
}

String ConfigManager::getDatabaseServer() {
    return _config["database"]["server"].as<String>();
}

void ConfigManager::setDatabaseServer(const String& server) {
    _config["database"]["server"] = server;
}

int ConfigManager::getDatabasePort() {
    return _config["database"]["port"].as<int>();
}

void ConfigManager::setDatabasePort(int port) {
    _config["database"]["port"] = port;
}

String ConfigManager::getDatabaseUsername() {
    return _config["database"]["username"].as<String>();
}

void ConfigManager::setDatabaseUsername(const String& username) {
    _config["database"]["username"] = username;
}

String ConfigManager::getDatabasePassword() {
    return _config["database"]["password"].as<String>();
}

void ConfigManager::setDatabasePassword(const String& password) {
    _config["database"]["password"] = password;
}

String ConfigManager::getDatabaseName() {
    return _config["database"]["name"].as<String>();
}

void ConfigManager::setDatabaseName(const String& name) {
    _config["database"]["name"] = name;
}

String ConfigManager::getAdminUsername() {
    return _config["admin"]["username"].as<String>();
}

void ConfigManager::setAdminUsername(const String& username) {
    _config["admin"]["username"] = username;
}

String ConfigManager::getAdminPassword() {
    return _config["admin"]["password"].as<String>();
}

void ConfigManager::setAdminPassword(const String& password) {
    _config["admin"]["password"] = password;
}
