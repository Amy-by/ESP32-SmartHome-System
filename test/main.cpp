#include "core/DataProcessor.h"
#include <iostream>
int main() {
    DataProcessor dp(5);
    for (int i = 1; i <= 10; ++i) dp.addData((float)i);
    std::cout << "count=" << dp.getDataCount() << "\n";
    std::cout << "avg=" << dp.getAverage() << "\n";
    std::cout << "min=" << dp.getMin() << "\n";
    std::cout << "max=" << dp.getMax() << "\n";
    std::cout << "std=" << dp.getStandardDeviation() << "\n";
    std::cout << "ma=" << dp.movingAverageFilter() << "\n";
    std::cout << "median=" << dp.medianFilter() << "\n";
    return 0;
}