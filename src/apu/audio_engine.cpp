#include "audio_engine.h"
#include <SDL2/SDL.h>

AudioEngine& AudioEngine::getInstance() {
    static AudioEngine instance;
    return instance;
}

void AudioEngine::pushSample(float sample) {
    samples.push_back(sample);
    if (samples.size() == sampleLimit) {
        SDL_QueueAudio(deviceId, samples.data(), sampleLimit * sizeof(float));
        samples.clear();
    }
}

AudioEngine::AudioEngine() {
    SDL_Init(SDL_INIT_AUDIO);
    samples.reserve(sampleLimit);
    SDL_AudioSpec spec = {}, have;
    spec.freq = 1 << 16;
    spec.format = AUDIO_F32SYS;
    spec.channels = 1;
    spec.samples = sampleLimit;
    deviceId = SDL_OpenAudioDevice(nullptr, 0, &spec, &have, 0);
    SDL_PauseAudioDevice(deviceId, 0);
}
