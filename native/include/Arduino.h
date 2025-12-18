#pragma once
#include <string>
#include <chrono>
#include <thread>
#include <cmath>
using String = std::string;
inline unsigned long millis() { static auto s = std::chrono::steady_clock::now(); auto n = std::chrono::steady_clock::now(); return (unsigned long)std::chrono::duration_cast<std::chrono::milliseconds>(n - s).count(); }
inline void delay(unsigned long ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }
