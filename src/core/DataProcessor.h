#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <Arduino.h>
#include <vector>
#include <deque>

/**
 * @brief 数据处理器类，负责数据滤波和统计分析
 * 提供移动平均滤波、统计分析等功能
 */
class DataProcessor {
public:
    /**
     * @brief 构造函数
     * @param windowSize 移动平均窗口大小，默认10
     */
    DataProcessor(int windowSize = 10);
    
    /**
     * @brief 析构函数
     */
    ~DataProcessor();
    
    /**
     * @brief 设置移动平均窗口大小
     * @param windowSize 窗口大小
     */
    void setWindowSize(int windowSize);
    
    /**
     * @brief 添加新数据到处理器
     * @param value 新数据值
     */
    void addData(float value);
    
    /**
     * @brief 使用移动平均滤波处理数据
     * @return 滤波后的数据值
     */
    float movingAverageFilter();
    
    /**
     * @brief 使用中值滤波处理数据
     * @return 滤波后的数据值
     */
    float medianFilter();
    
    /**
     * @brief 获取数据的平均值
     * @return 数据平均值
     */
    float getAverage();
    
    /**
     * @brief 获取数据的最大值
     * @return 数据最大值
     */
    float getMax();
    
    /**
     * @brief 获取数据的最小值
     * @return 数据最小值
     */
    float getMin();
    
    /**
     * @brief 获取数据的标准差
     * @return 数据标准差
     */
    float getStandardDeviation();
    
    /**
     * @brief 获取数据的方差
     * @return 数据方差
     */
    float getVariance();
    
    /**
     * @brief 获取当前数据数量
     * @return 数据数量
     */
    int getDataCount();
    
    /**
     * @brief 清除所有数据
     */
    void clearData();
    
private:
    std::deque<float> dataBuffer;  // 数据缓冲区
    int windowSize;                // 移动平均窗口大小
    
    /**
     * @brief 更新数据缓冲区，保持窗口大小
     */
    void updateBuffer();
};

#endif // DATAPROCESSOR_H