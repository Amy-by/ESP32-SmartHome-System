#include "DataProcessor.h"
#include <cmath>

/**
 * @brief 构造函数
 * @param windowSize 移动平均窗口大小，默认10
 */
DataProcessor::DataProcessor(int windowSize) : windowSize(windowSize) {
}

/**
 * @brief 析构函数
 */
DataProcessor::~DataProcessor() {
    clearData();
}

/**
 * @brief 设置移动平均窗口大小
 * @param windowSize 窗口大小
 */
void DataProcessor::setWindowSize(int windowSize) {
    this->windowSize = windowSize;
    updateBuffer();
}

/**
 * @brief 添加新数据到处理器
 * @param value 新数据值
 */
void DataProcessor::addData(float value) {
    dataBuffer.push_back(value);
    updateBuffer();
}

/**
 * @brief 使用移动平均滤波处理数据
 * @return 滤波后的数据值
 */
float DataProcessor::movingAverageFilter() {
    if (dataBuffer.empty()) {
        return 0.0f;
    }
    
    float sum = 0.0f;
    for (float value : dataBuffer) {
        sum += value;
    }
    
    return sum / dataBuffer.size();
}

/**
 * @brief 使用中值滤波处理数据
 * @return 滤波后的数据值
 */
float DataProcessor::medianFilter() {
    if (dataBuffer.empty()) {
        return 0.0f;
    }
    
    // 创建数据副本并排序
    std::vector<float> sortedData(dataBuffer.begin(), dataBuffer.end());
    
    // 冒泡排序
    for (size_t i = 0; i < sortedData.size() - 1; i++) {
        for (size_t j = 0; j < sortedData.size() - i - 1; j++) {
            if (sortedData[j] > sortedData[j + 1]) {
                std::swap(sortedData[j], sortedData[j + 1]);
            }
        }
    }
    
    // 计算中值
    size_t middle = sortedData.size() / 2;
    if (sortedData.size() % 2 == 0) {
        // 偶数个元素，取中间两个的平均值
        return (sortedData[middle - 1] + sortedData[middle]) / 2.0f;
    } else {
        // 奇数个元素，取中间的
        return sortedData[middle];
    }
}

/**
 * @brief 获取数据的平均值
 * @return 数据平均值
 */
float DataProcessor::getAverage() {
    return movingAverageFilter(); // 复用移动平均滤波的计算
}

/**
 * @brief 获取数据的最大值
 * @return 数据最大值
 */
float DataProcessor::getMax() {
    if (dataBuffer.empty()) {
        return 0.0f;
    }
    
    float maxValue = dataBuffer.front();
    for (float value : dataBuffer) {
        if (value > maxValue) {
            maxValue = value;
        }
    }
    
    return maxValue;
}

/**
 * @brief 获取数据的最小值
 * @return 数据最小值
 */
float DataProcessor::getMin() {
    if (dataBuffer.empty()) {
        return 0.0f;
    }
    
    float minValue = dataBuffer.front();
    for (float value : dataBuffer) {
        if (value < minValue) {
            minValue = value;
        }
    }
    
    return minValue;
}

/**
 * @brief 获取数据的标准差
 * @return 数据标准差
 */
float DataProcessor::getStandardDeviation() {
    return sqrt(getVariance());
}

/**
 * @brief 获取数据的方差
 * @return 数据方差
 */
float DataProcessor::getVariance() {
    if (dataBuffer.empty() || dataBuffer.size() == 1) {
        return 0.0f;
    }
    
    float average = getAverage();
    float variance = 0.0f;
    
    for (float value : dataBuffer) {
        variance += pow(value - average, 2);
    }
    
    return variance / (dataBuffer.size() - 1); // 使用无偏方差
}

/**
 * @brief 获取当前数据数量
 * @return 数据数量
 */
int DataProcessor::getDataCount() {
    return dataBuffer.size();
}

/**
 * @brief 清除所有数据
 */
void DataProcessor::clearData() {
    dataBuffer.clear();
}

/**
 * @brief 更新数据缓冲区，保持窗口大小
 */
void DataProcessor::updateBuffer() {
    // 如果缓冲区大小超过窗口大小，移除最旧的数据
    while (dataBuffer.size() > windowSize) {
        dataBuffer.pop_front();
    }
}