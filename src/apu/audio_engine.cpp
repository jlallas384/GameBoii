#include "audio_engine.h"
#include <SDL2/SDL.h>

AudioEngine& AudioEngine::getInstance() {
    static AudioEngine instance;
    return instance;
}

void AudioEngine::pushSample(float left, float right) {
    samples.push_back(left);
    samples.push_back(right);
    if (samples.size() == sampleLimit * 2) {
        SDL_QueueAudio(deviceId, samples.data(), sampleLimit * sizeof(float) * 2);
        samples.clear();
    }
}

AudioEngine::AudioEngine() {
    SDL_Init(SDL_INIT_AUDIO);
    samples.reserve(sampleLimit);
    SDL_AudioSpec spec = {}, have;
    spec.freq = 1 << 16;
    spec.format = AUDIO_F32SYS;
    spec.channels = 2;
    spec.samples = sampleLimit;
    deviceId = SDL_OpenAudioDevice(nullptr, 0, &spec, &have, 0);
    SDL_PauseAudioDevice(deviceId, 0);
}
