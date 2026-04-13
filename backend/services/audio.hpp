#pragma once

#include <SDL3/SDL_audio.h>
#include "../core/definitions.hpp"
#include "../core/node/node.hpp"
#include "../core/node/components.hpp"
#include "../utils/logger.hpp"
#include "../core/signal.hpp"

static SDL_AudioDeviceID audio_device = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK;

class Audio
{
public:
    Audio();
    ~Audio();

    void openAudioDevice(int id);
    SDL_AudioDeviceID getAudioDevice();

    void initialize();
    void shutdown();
private:
    SDL_AudioDeviceID device;
};

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
 * The source of a one-dimensional sound. For now, you can only load WAV files because of SDL audio streams.
 * You can modify its pitch and volume and loop it. It also has a Signal for when the sound finishes playing.
 *
 * Example Usage:
 * ```
 * std::shared_ptr<Node> node = std::make_shared<Node>();
 * node->transform.scale({4, 4});
 * std::shared_ptr<AudioSource> sound = std::make_shared<AudioSource>("assets/sound.wav");
 * node->addComponent(sound);
 *
 * // play sound
 * sound->play();
 * ```
 *
 * Make sure to also check out the audio example in the examples folder!
 */
class AudioSource : public Component
{
public:
    AudioSource(std::string path);
    std::string path;
    float volume = 1.0f;
    float pitch = 1.0f;
    bool playing = false;
    bool loop = false;

    Signal<> finished;

    void play();
    bool loadAudio();
    void initialize();
    void update(float deltaTime);
    void shutdown();

private:
    Sound handle;
    float prevVolume = 0.0f;
    float prevPitch = 0.0f;
};