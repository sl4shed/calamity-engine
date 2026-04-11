#include "audio.hpp"
#include <SDL3/SDL_audio.h>

AudioSource::AudioSource(std::string path)
{
    this->path = path;
}

bool AudioSource::loadAudio()
{
    char *wav_path = nullptr;

    if (this->handle.stream != nullptr)
    {
        Logger::warn("Can't load audiosource twice type shi");
        return false;
    }

    SDL_AudioSpec spec;
    audio_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    SDL_asprintf(&wav_path, "%s%s", SDL_GetBasePath(), path.c_str());
    if (!SDL_LoadWAV(wav_path, &spec, &this->handle.wav_data, &this->handle.wav_data_len))
    {
        Logger::error("Couldn't load audio file: {}", SDL_GetError());
        return false;
    }

    this->handle.stream = SDL_CreateAudioStream(&spec, nullptr);
    if (!this->handle.stream)
    {
        Logger::error("Couldn't create audio stream: {}", SDL_GetError());
        return false;
    }
    if (!SDL_BindAudioStream(audio_device, this->handle.stream))
    {
        Logger::error("Failed to bind {} stream to device: {}", this->path, SDL_GetError());
        return false;
    }
    Logger::debug("Loaded successfully.");
    SDL_free(wav_path);
    return true;
}

// TODO: add stopping/resetting the audio stream maybe.
// TODO: add a finished variable like in godot
void AudioSource::update(float deltaTime)
{
    if (this->volume != this->prevVolume)
    {
        SDL_SetAudioStreamGain(this->handle.stream, this->volume);
        this->prevVolume = this->volume;
    }

    if (this->playing != this->prevPlaying)
    {
        if (this->playing == false) {
            SDL_PauseAudioStreamDevice(this->handle.stream);
        }
        else
        {
            SDL_ResumeAudioStreamDevice(this->handle.stream);
        }
    }

    if(this->pitch != this->prevPitch) {
        SDL_SetAudioStreamFrequencyRatio(this->handle.stream, this->pitch);
        this->prevPitch = this->pitch;
    }

    // update stream data if needed
    if (SDL_GetAudioStreamQueued(this->handle.stream) < (int)this->handle.wav_data_len)
    {
        SDL_PutAudioStreamData(this->handle.stream, this->handle.wav_data, (int)this->handle.wav_data_len);
    }
}

void AudioSource::exit()
{
    if (this->handle.stream) {
        SDL_DestroyAudioStream(this->handle.stream);
        SDL_free(this->handle.wav_data);
        this->handle.stream = nullptr;
        this->handle.wav_data = nullptr;
    }
}

AudioSource::~AudioSource()
{
    exit();
}

void AudioSource::initialize()
{
    this->loadAudio();
}