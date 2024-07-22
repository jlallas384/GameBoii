#pragma once

#include <chrono>

template <size_t TicksPerSecond>
class Timer {
public:
    Timer() : last(std::chrono::system_clock::now()) {}
    double elapsed() {
        auto now = std::chrono::system_clock::now();
        double ret = std::chrono::duration<double, std::ratio<1, TicksPerSecond>>(now - last).count();
        last = now;
        return ret;
    }
private:
    std::chrono::system_clock::time_point last;
};