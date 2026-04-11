#pragma once

#include <SDL3/SDL_audio.h>
#include "../core/definitions.hpp"
#include "../core/node/node.hpp"
#include "../core/node/components.hpp"
#include "../utils/logger.hpp"

static SDL_AudioDeviceID audio_device = 0;

/**
 * Represents a digital sound.
 */
typedef struct Sound
{
    Uint8 *wav_data;
    Uint32 wav_data_len;
    SDL_AudioStream *stream;
} Sound;

/**
 * # AudioSource
 * The source of a one-dimensional sound.
 * You can modify its pitch and volume.
 *
 * Example Usage:
 * ```
 * std::shared_ptr<Node> node = std::make_shared<Node>();
 * node->transform.scale({4, 4});
 * std::shared_ptr<AudioSource> sound = std::make_shared<AudioSource>("assets/sound.wav");
 * node->addComponent(sound);
 *
 * // play sound
 * sound->playing = true;
 * ```
 *
 * Make sure to also check out the audio example in the examples folder!
 */
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
    void exit();

private:
    Sound handle;
    float prevVolume = 0.0f;
    float prevPitch = 0.0f;
    bool prevPlaying = true;
};