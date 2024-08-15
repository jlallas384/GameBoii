#pragma once

#include <fstream>
#include <vector>
#include <array>

template <typename T>
constexpr T setBit(T num, int bit, bool on) {
    return (num & ~(1 << bit)) | (on << bit);
}

template <typename T>
constexpr bool getBit(T num, int bit) {
    return num & (1 << bit);
}

template <typename T>
void serialize(std::ofstream& os, const T& value) {
    os.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template <typename T>
void serialize(std::ofstream& os, const std::vector<T>& vec) {
    for (const auto& value : vec) {
        serialize(os, value);
    }
}

template <typename T, size_t N>
void serialize(std::ofstream& os, const std::array<T, N>& arr) {
    for (const auto& value : arr) {
        serialize(os, value);
    }
}

template <typename T>
void deserialize(std::ifstream& is, T& value) {
    is.read(reinterpret_cast<char*>(&value), sizeof(T));
}

template <typename T>
void deserialize(std::ifstream& is, std::vector<T>& vec) {
    for (auto& value : vec) {
        deserialize(is, value);
    }
}

template <typename T, size_t N>
void deserialize(std::ifstream& is, std::array<T, N>& arr) {
    for (auto& value : arr) {
        deserialize(is, value);
    }
}