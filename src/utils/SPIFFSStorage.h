#ifndef SPIFFSSTORAGE_H
#define SPIFFSSTORAGE_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "config.h"

class SPIFFSStorage {
public:
    /**
     * @brief 构造函数
     */
    SPIFFSStorage();
    
    /**
     * @brief 析构函数
     */
    ~SPIFFSStorage();
    
    /**
     * @brief 初始化SPIFFS文件系统
     * @return bool 初始化是否成功
     */
    bool init();
    
    /**
     * @brief 读取文件内容
     * @param filePath 文件路径
     * @param content 文件内容引用
     * @return bool 读取是否成功
     */
    bool readFile(const String &filePath, String &content);
    
    /**
     * @brief 写入文件内容
     * @param filePath 文件路径
     * @param content 文件内容
     * @param append 是否追加内容（默认覆盖）
     * @return bool 写入是否成功
     */
    bool writeFile(const String &filePath, const String &content, bool append = false);
    
    /**
     * @brief 读取JSON文件
     * @param filePath 文件路径
     * @param doc JsonDocument引用
     * @return bool 读取是否成功
     */
    bool readJsonFile(const String &filePath, JsonDocument &doc);
    
    /**
     * @brief 写入JSON文件
     * @param filePath 文件路径
     * @param doc JsonDocument
     * @return bool 写入是否成功
     */
    bool writeJsonFile(const String &filePath, const JsonDocument &doc);
    
    /**
     * @brief 删除文件
     * @param filePath 文件路径
     * @return bool 删除是否成功
     */
    bool deleteFile(const String &filePath);
    
    /**
     * @brief 重命名文件
     * @param oldPath 旧文件路径
     * @param newPath 新文件路径
     * @return bool 重命名是否成功
     */
    bool renameFile(const String &oldPath, const String &newPath);
    
    /**
     * @brief 检查文件是否存在
     * @param filePath 文件路径
     * @return bool 文件是否存在
     */
    bool fileExists(const String &filePath);
    
    /**
     * @brief 获取文件大小
     * @param filePath 文件路径
     * @return size_t 文件大小（字节），如果文件不存在返回0
     */
    size_t getFileSize(const String &filePath);
    
    /**
     * @brief 获取SPIFFS总大小
     * @return size_t 总大小（字节）
     */
    size_t getTotalSize();
    
    /**
     * @brief 获取SPIFFS已用大小
     * @return size_t 已用大小（字节）
     */
    size_t getUsedSize();
    
    /**
     * @brief 获取SPIFFS可用大小
     * @return size_t 可用大小（字节）
     */
    size_t getFreeSize();
    
    /**
     * @brief 格式化SPIFFS文件系统（注意：这会删除所有文件）
     * @return bool 格式化是否成功
     */
    bool format();
    
    /**
     * @brief 列出目录内容
     * @param dirPath 目录路径
     * @param files 文件列表引用
     * @param recursive 是否递归列出子目录
     * @return bool 列出是否成功
     */
    bool listDirectory(const String &dirPath, JsonArray &files, bool recursive = false);
    
private:
    /**
     * @brief 递归列出目录内容（内部使用）
     * @param dir 目录对象
     * @param files 文件列表引用
     * @param recursive 是否递归
     */
    void listDirectoryRecursive(File dir, JsonArray &files, bool recursive);
    
    bool _initialized;
};

#endif // SPIFFSSTORAGE_H
