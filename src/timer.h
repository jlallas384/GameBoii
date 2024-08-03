#pragma once

#include <chrono>
#include <cstdint>

template <size_t TicksPerSecond>
class Timer {
public:
    Timer() : last(std::chrono::system_clock::now()) {}
    uint64_t elapsed() {
        auto now = std::chrono::system_clock::now();
        std::chrono::duration<double, std::ratio<1, TicksPerSecond>> ret = now - last;
        last = now;
        return std::chrono::ceil<std::chrono::duration<uint64_t, std::ratio<1, TicksPerSecond>>>(ret).count();
    }
private:
    std::chrono::system_clock::time_point last;
};