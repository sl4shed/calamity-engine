#pragma once

#include <SDL3/SDL_audio.h>
#include "definitions.hpp"
#include "node.hpp"
#include "components.hpp"
#include "logger.hpp"

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
    AudioSource(std::string path, float volume);
    ~AudioSource();
    std::string path;
    float volume;

    void pause();
    void play();

    bool loadAudio();
    void initialize();
    void update();

private:
    Sound handle;
};