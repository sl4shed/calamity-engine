#pragma once

#include <SDL3/SDL_audio.h>
#include "../core/definitions.hpp"
#include "../core/node/node.hpp"
#include "../core/node/components.hpp"
#include "../utils/logger.hpp"

static SDL_AudioDeviceID audio_device = 0;

typedef struct Sound
{
    Uint8 *wav_data;
    Uint32 wav_data_len;
    SDL_AudioStream *stream;
} Sound;

class AudioSource : public Component
{
public:
    AudioSource(std::string path);
    ~AudioSource();
    std::string path;
    float volume = 1.0f;
    float pitch = 1.0f;
    bool playing = false;

    bool loadAudio();
    void initialize();
    void update(float deltaTime);

private:
    Sound handle;
    float prevVolume = 0.0f;
    float prevPitch = 0.0f;
    bool prevPlaying = true;
};