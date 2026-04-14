#include "audio.hpp"
#include <SDL3/SDL_audio.h>


Audio::Audio() {}

Audio::~Audio() {
    SDL_CloseAudioDevice(this->device);
}

void Audio::initialize()
{
    this->openAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);
}

void Audio::openAudioDevice(int id) {
    this->device = SDL_OpenAudioDevice(id, nullptr);
    if (this->device == 0) {
        Logger::error("Failed to open audio device: {}", SDL_GetError());
    }
}

SDL_AudioDeviceID Audio::getAudioDevice() {
    return this->device;
}

AudioSource::AudioSource(std::string path)
{
    this->path = path;
}

void AudioSource::play() {
    this->playing = true;
    SDL_FlushAudioStream(this->handle.stream);
    SDL_PutAudioStreamData(this->handle.stream, this->handle.wav_data, (int)this->handle.wav_data_len);
    SDL_ResumeAudioStreamDevice(this->handle.stream);
    this->finished.reset();
}

bool AudioSource::loadAudio()
{
    char *wav_path = nullptr;

    if (this->handle.stream != nullptr)
    {
        Logger::warn("can't load audiosource twice type shi");
        return false;
    }

    SDL_AudioSpec spec;
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
    if (!SDL_BindAudioStream(Services::audio()->getAudioDevice(), this->handle.stream))
    {
        Logger::error("Failed to bind {} stream to device: {}", this->path, SDL_GetError());
        return false;
    }

    SDL_free(wav_path);
    return true;
}

void AudioSource::update(float deltaTime)
{
    if (this->volume != this->prevVolume)
    {
        SDL_SetAudioStreamGain(this->handle.stream, this->volume);
        this->prevVolume = this->volume;
    }

    if(this->pitch != this->prevPitch) {
        SDL_SetAudioStreamFrequencyRatio(this->handle.stream, this->pitch);
        this->prevPitch = this->pitch;
    }

    if (!this->loop && SDL_GetAudioStreamQueued(this->handle.stream) == 0) {
        this->playing = false;
        finished.fire();
    } else if (SDL_GetAudioStreamQueued(this->handle.stream) < (int)this->handle.wav_data_len) {
        SDL_PutAudioStreamData(this->handle.stream, this->handle.wav_data, (int)this->handle.wav_data_len);
    }
}

void AudioSource::shutdown()
{
    if (this->handle.stream) {
        SDL_DestroyAudioStream(this->handle.stream);
        SDL_free(this->handle.wav_data);
        this->handle.stream = nullptr;
        this->handle.wav_data = nullptr;
    }
}

void AudioSource::initialize()
{
    this->loadAudio();
}