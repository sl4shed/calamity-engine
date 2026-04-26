#pragma once

#include <SDL3/SDL_audio.h>
#include "../core/definitions.hpp"
#include "../core/node/node.hpp"
#include "../core/node/components.hpp"
#include "../utils/logger.hpp"
#include "../core/signal.hpp"
#include "backend/utils/file.hpp"

static SDL_AudioDeviceID audio_device = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK;

class Audio
{
public:
    Audio();
    ~Audio();

    static const int bufferSize = 4096 * 4;
    static const int chunkSize = 4096;
    void openAudioDevice(int id);
    SDL_AudioDeviceID getAudioDevice() const;

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
    Uint8 *wav_data = nullptr;
    Uint32 wav_data_len = 0;
    SDL_AudioStream *stream = nullptr;
} Sound;

/**
 * # AudioSource
 * The source of a one-dimensional sound. For now, you can only load WAV files because of SDL audio streams.
 * You can modify its pitch and volume and loop it. It also has a Signal for when the sound finishes playing.
 *
 * Example Usage:
 * ```
 * std::shared_ptr<Node> node = std::make_shared<Node>();
 * node->addComponent(std::make_shared<AudioSource>("assets/sound.wav"));
 *
 * // play sound (make sure to do this AFTER the audio source was initialized)
 * sound->play();
 * ```
 *
 * Make sure to also check out the audio example in the examples folder!
 */
class AudioSource : public Component
{
public:
    AudioSource(const std::string& path);
    std::string path;

    float getVolume() const;
    float getPitch() const;
    bool getPlaying() const;
    void setPitch(float pitch);
    void setVolume(float volume);

    bool loop = false;
    Signal<> finished;
    Signal<> looped;
    Signal<> stopped;
    Signal<> paused;

    void play();
    void stop();
    void pause();
    bool loadAudio();
    void update(float deltaTime);
    void shutdown();

    template <class Archive>
    void save(Archive &ar) const
    {
        ar(CEREAL_NVP(volume), CEREAL_NVP(pitch), CEREAL_NVP(playing), CEREAL_NVP(path), CEREAL_NVP(loop));
    }

    template <class Archive>
    void load(Archive &ar)
    {
        ar(CEREAL_NVP(volume), CEREAL_NVP(pitch), CEREAL_NVP(playing), CEREAL_NVP(path), CEREAL_NVP(loop));
        this->fsPath = File::getAbsoluteFilePath(path);
        loadAudio();
        SDL_SetAudioStreamFrequencyRatio(this->handle.stream, this->pitch);
        SDL_SetAudioStreamGain(this->handle.stream, this->volume);
        if (playing)
        {
            play();
        }
    }

private:
    bool playing = false;
    float volume = 1.0f;
    float pitch = 1.0f;
    size_t cursor = 0;
    bool finishedFeeding = false;

    std::string fsPath;
    Sound handle;
};