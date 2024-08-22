#pragma once

#include <vector>
#include <cstdint>

class AudioEngine {
public:
    void pushSample(float sample);
    static AudioEngine& getInstance();
private:
    AudioEngine();
    std::vector<float> samples;
    uint32_t deviceId = 0;
    static constexpr auto sampleLimit = 4096;
};