#include "DataLogger.h"

DataLogger::DataLogger(SPIFFSStorage *storage, const String &logFilePath, size_t maxLogSize) {
    _storage = storage;
    _logFilePath = logFilePath;
    _maxLogSize = maxLogSize;
    _initialized = false;
}

DataLogger::~DataLogger() {
    // 不需要特殊清理
}

bool DataLogger::init() {
    if (!_initialized) {
        if (_storage == nullptr) {
            return false;
        }
        
        // 检查SPIFFS是否已初始化
        if (!_storage->init()) {
            return false;
        }
        
        // 检查日志文件是否存在，如果不存在则创建
        if (!_storage->fileExists(_logFilePath)) {
            if (!_storage->writeFile(_logFilePath, "", false)) {
                return false;
            }
        }
        
        _initialized = true;
    }
    return true;
}

bool DataLogger::log(const String &level, const String &message) {
    if (!_initialized || _storage == nullptr) {
        return false;
    }
    
    // 检查日志级别是否有效
    if (level != "INFO" && level != "WARNING" && level != "ERROR" && level != "ALARM") {
        return false;
    }
    
    // 构建日志消息
    String timestamp = getTimestamp();
    String logMessage = "[" + timestamp + "] [" + level + "] " + message + "\n";
    
    // 检查并调整日志大小
    if (!checkAndAdjustLogSize()) {
        return false;
    }
    
    // 写入日志
    return _storage->writeFile(_logFilePath, logMessage, true);
}

bool DataLogger::info(const String &message) {
    return log("INFO", message);
}

bool DataLogger::warning(const String &message) {
    return log("WARNING", message);
}

bool DataLogger::error(const String &message) {
    return log("ERROR", message);
}

bool DataLogger::alarm(const String &message) {
    return log("ALARM", message);
}

bool DataLogger::readLogs(String &content) {
    if (!_initialized || _storage == nullptr) {
        return false;
    }
    
    return _storage->readFile(_logFilePath, content);
}

bool DataLogger::clearLogs() {
    if (!_initialized || _storage == nullptr) {
        return false;
    }
    
    // 清空日志文件（写入空内容）
    return _storage->writeFile(_logFilePath, "", false);
}

size_t DataLogger::getCurrentLogSize() {
    if (!_initialized || _storage == nullptr) {
        return 0;
    }
    
    return _storage->getFileSize(_logFilePath);
}

size_t DataLogger::getMaxLogSize() {
    return _maxLogSize;
}

void DataLogger::setMaxLogSize(size_t maxSize) {
    _maxLogSize = maxSize;
    
    // 如果当前日志大小超过新的最大大小，调整日志
    if (_initialized) {
        checkAndAdjustLogSize();
    }
}

bool DataLogger::checkAndAdjustLogSize() {
    if (!_initialized || _storage == nullptr) {
        return false;
    }
    
    size_t currentSize = _storage->getFileSize(_logFilePath);
    
    // 如果当前日志大小小于最大大小，不需要调整
    if (currentSize < _maxLogSize) {
        return true;
    }
    
    // 读取当前日志内容
    String content;
    if (!_storage->readFile(_logFilePath, content)) {
        return false;
    }
    
    // 计算需要保留的内容大小（保留后半部分）
    size_t keepSize = _maxLogSize * 0.7; // 保留70%的最大大小
    if (keepSize == 0) {
        keepSize = 1024; // 最小保留1KB
    }
    
    // 找到合适的换行符位置，避免截断日志行
    int startIndex = content.length() - keepSize;
    if (startIndex < 0) {
        startIndex = 0;
    }
    
    // 找到第一个完整的日志行
    for (int i = startIndex; i < content.length(); i++) {
        if (content.charAt(i) == '\n') {
            startIndex = i + 1;
            break;
        }
    }
    
    // 保留后半部分日志
    String newContent = content.substring(startIndex);
    
    // 写入新的日志内容
    return _storage->writeFile(_logFilePath, newContent, false);
}

String DataLogger::getTimestamp() {
    // 使用ESP32的内部时钟（注意：如果没有NTP同步，时间可能不准确）
    unsigned long currentMillis = millis();
    unsigned long seconds = currentMillis / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    
    // 格式：HH:MM:SS
    char timestamp[9];
    sprintf(timestamp, "%02lu:%02lu:%02lu", hours % 24, minutes % 60, seconds % 60);
    
    return String(timestamp);
}
