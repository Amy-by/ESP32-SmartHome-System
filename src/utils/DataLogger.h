#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <Arduino.h>
#include "SPIFFSStorage.h"
#include "config.h"

class DataLogger {
public:
    /**
     * @brief 构造函数
     * @param storage SPIFFSStorage对象指针
     * @param logFilePath 日志文件路径
     * @param maxLogSize 最大日志大小（字节）
     */
    DataLogger(SPIFFSStorage *storage, const String &logFilePath, size_t maxLogSize = DEFAULT_MAX_LOG_SIZE);
    
    /**
     * @brief 析构函数
     */
    ~DataLogger();
    
    /**
     * @brief 初始化日志系统
     * @return bool 初始化是否成功
     */
    bool init();
    
    /**
     * @brief 记录日志
     * @param level 日志级别（INFO, WARNING, ERROR, ALARM）
     * @param message 日志消息
     * @return bool 记录是否成功
     */
    bool log(const String &level, const String &message);
    
    /**
     * @brief 记录信息日志
     * @param message 日志消息
     * @return bool 记录是否成功
     */
    bool info(const String &message);
    
    /**
     * @brief 记录警告日志
     * @param message 日志消息
     * @return bool 记录是否成功
     */
    bool warning(const String &message);
    
    /**
     * @brief 记录错误日志
     * @param message 日志消息
     * @return bool 记录是否成功
     */
    bool error(const String &message);
    
    /**
     * @brief 记录告警日志
     * @param message 日志消息
     * @return bool 记录是否成功
     */
    bool alarm(const String &message);
    
    /**
     * @brief 读取日志内容
     * @param content 日志内容引用
     * @return bool 读取是否成功
     */
    bool readLogs(String &content);
    
    /**
     * @brief 清空日志
     * @return bool 清空是否成功
     */
    bool clearLogs();
    
    /**
     * @brief 获取当前日志大小
     * @return size_t 当前日志大小（字节）
     */
    size_t getCurrentLogSize();
    
    /**
     * @brief 获取最大日志大小
     * @return size_t 最大日志大小（字节）
     */
    size_t getMaxLogSize();
    
    /**
     * @brief 设置最大日志大小
     * @param maxSize 最大日志大小（字节）
     */
    void setMaxLogSize(size_t maxSize);
    
private:
    /**
     * @brief 检查并调整日志大小
     * @return bool 调整是否成功
     */
    bool checkAndAdjustLogSize();
    
    /**
     * @brief 获取当前时间戳
     * @return String 格式化的时间戳字符串
     */
    String getTimestamp();
    
    SPIFFSStorage *_storage; ///< SPIFFS存储对象指针
    String _logFilePath;      ///< 日志文件路径
    size_t _maxLogSize;       ///< 最大日志大小（字节）
    bool _initialized;        ///< 初始化状态
};

#endif // DATALOGGER_H
