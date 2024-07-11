#pragma once

template <typename T>
constexpr T setBit(T num, int bit, bool on) {
    return (num & ~(1 << bit)) | (on << bit);
}

template <typename T>
constexpr bool getBit(T num, int bit) {
    return num & (1 << bit);
}