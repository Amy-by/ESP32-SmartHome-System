#include "SPIFFSStorage.h"

SPIFFSStorage::SPIFFSStorage() {
    _initialized = false;
}

SPIFFSStorage::~SPIFFSStorage() {
    // SPIFFS不需要手动关闭，会自动处理
}

bool SPIFFSStorage::init() {
    if (!_initialized) {
        // 初始化SPIFFS
        if (!SPIFFS.begin(true)) { // true表示格式化（如果需要）
            return false;
        }
        _initialized = true;
    }
    return true;
}

bool SPIFFSStorage::readFile(const String &filePath, String &content) {
    if (!_initialized) {
        return false;
    }
    
    File file = SPIFFS.open(filePath, FILE_READ);
    if (!file) {
        return false;
    }
    
    content = file.readString();
    file.close();
    
    return true;
}

bool SPIFFSStorage::writeFile(const String &filePath, const String &content, bool append) {
    if (!_initialized) {
        return false;
    }
    
    // 检查文件大小是否超过限制
    if (!append) {
        if (content.length() > SPIFFS_MAX_FILE_SIZE) {
            return false;
        }
    }
    
    File file;
    if (append) {
        // 追加模式
        file = SPIFFS.open(filePath, FILE_APPEND);
        if (!file) {
            // 如果文件不存在，尝试创建新文件
            file = SPIFFS.open(filePath, FILE_WRITE);
            if (!file) {
                return false;
            }
        }
        
        // 检查追加后的文件大小是否超过限制
        if (file.size() + content.length() > SPIFFS_MAX_FILE_SIZE) {
            file.close();
            return false;
        }
    } else {
        // 覆盖模式
        file = SPIFFS.open(filePath, FILE_WRITE);
        if (!file) {
            return false;
        }
    }
    
    if (file.print(content) != content.length()) {
        file.close();
        return false;
    }
    
    file.close();
    return true;
}

bool SPIFFSStorage::readJsonFile(const String &filePath, JsonDocument &doc) {
    if (!_initialized) {
        return false;
    }
    
    File file = SPIFFS.open(filePath, FILE_READ);
    if (!file) {
        return false;
    }
    
    // 解析JSON文件
    DeserializationError error = deserializeJson(doc, file);
    file.close();
    
    return error == DeserializationError::Ok;
}

bool SPIFFSStorage::writeJsonFile(const String &filePath, const JsonDocument &doc) {
    if (!_initialized) {
        return false;
    }
    
    File file = SPIFFS.open(filePath, FILE_WRITE);
    if (!file) {
        return false;
    }
    
    // 序列化JSON到文件
    size_t written = serializeJson(doc, file);
    file.close();
    
    return written > 0;
}

bool SPIFFSStorage::deleteFile(const String &filePath) {
    if (!_initialized) {
        return false;
    }
    
    return SPIFFS.remove(filePath);
}

bool SPIFFSStorage::renameFile(const String &oldPath, const String &newPath) {
    if (!_initialized) {
        return false;
    }
    
    return SPIFFS.rename(oldPath, newPath);
}

bool SPIFFSStorage::fileExists(const String &filePath) {
    if (!_initialized) {
        return false;
    }
    
    return SPIFFS.exists(filePath);
}

size_t SPIFFSStorage::getFileSize(const String &filePath) {
    if (!_initialized) {
        return 0;
    }
    
    File file = SPIFFS.open(filePath, FILE_READ);
    if (!file) {
        return 0;
    }
    
    size_t size = file.size();
    file.close();
    
    return size;
}

size_t SPIFFSStorage::getTotalSize() {
    if (!_initialized) {
        return 0;
    }
    
    return SPIFFS.totalBytes();
}

size_t SPIFFSStorage::getUsedSize() {
    if (!_initialized) {
        return 0;
    }
    
    return SPIFFS.usedBytes();
}

size_t SPIFFSStorage::getFreeSize() {
    if (!_initialized) {
        return 0;
    }
    
    return SPIFFS.totalBytes() - SPIFFS.usedBytes();
}

bool SPIFFSStorage::format() {
    if (!_initialized) {
        return false;
    }
    
    SPIFFS.end();
    _initialized = false;
    
    // 格式化SPIFFS
    if (!SPIFFS.begin(true)) {
        return false;
    }
    
    _initialized = true;
    return true;
}

bool SPIFFSStorage::listDirectory(const String &dirPath, JsonArray &files, bool recursive) {
    if (!_initialized) {
        return false;
    }
    
    File dir = SPIFFS.open(dirPath);
    if (!dir || !dir.isDirectory()) {
        return false;
    }
    
    listDirectoryRecursive(dir, files, recursive);
    dir.close();
    
    return true;
}

void SPIFFSStorage::listDirectoryRecursive(File dir, JsonArray &files, bool recursive) {
    File entry = dir.openNextFile();
    while (entry) {
        JsonObject fileInfo = files.createNestedObject();
        fileInfo["name"] = entry.name();
        fileInfo["size"] = entry.size();
        fileInfo["isDirectory"] = entry.isDirectory();
        fileInfo["path"] = entry.path();
        
        if (entry.isDirectory() && recursive) {
            // 递归列出子目录
            JsonArray subFiles = fileInfo.createNestedArray("children");
            listDirectoryRecursive(entry, subFiles, recursive);
        }
        
        entry.close();
        entry = dir.openNextFile();
    }
}
