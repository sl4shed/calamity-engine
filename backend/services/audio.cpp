#include "audio.hpp"
#include <SDL3/SDL_audio.h>

#include "backend/utils/file.hpp"


Audio::Audio()
{
    this->openAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);
}

Audio::~Audio() {
    SDL_CloseAudioDevice(this->device);
}

void Audio::initialize() {}

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
    this->fsPath = File::getAbsoluteFilePath(path);
}

void AudioSource::play() {
    this->playing = true;

    if (this->handle.stream)
    {
        SDL_FlushAudioStream(this->handle.stream);
    } else
    {
        loadAudio();
    }

    SDL_ResumeAudioStreamDevice(this->handle.stream);
    this->finished.reset();
}

bool AudioSource::loadAudio()
{
    if (this->handle.stream != nullptr)
    {
        SDL_DestroyAudioStream(this->handle.stream);
        this->handle.wav_data = nullptr;
        this->handle.wav_data_len = 0;
    }

    SDL_AudioSpec spec;
    if (!SDL_LoadWAV(fsPath.c_str(), &spec, &this->handle.wav_data, &this->handle.wav_data_len))
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

    return true;
}

void AudioSource::setVolume(float _volume)
{
    this->volume = _volume;
    SDL_SetAudioStreamGain(this->handle.stream, this->volume);
}

void AudioSource::setPitch(float _pitch)
{
    this->pitch = _pitch;
    SDL_SetAudioStreamFrequencyRatio(this->handle.stream, this->pitch);
}

float AudioSource::getPitch()
{
    return pitch;
}

float AudioSource::getVolume()
{
    return volume;
}

bool AudioSource::getPlaying()
{
    return playing;
}

void AudioSource::update(float deltaTime)
{
    // ok here the loop is making sure that the audio stream data never dips below the wav data length
    // which is kinda aids
    // i should probably just have a buffer length
    // and feed it more data based on the buffer
    int queued = SDL_GetAudioStreamQueued(this->handle.stream);

    if (!finishedFeeding)
    {
        while (queued < Audio::bufferSize)
        {
            size_t remaining = this->handle.wav_data_len - cursor;
            size_t toCopy = std::min((size_t)Audio::chunkSize, remaining);

            SDL_PutAudioStreamData(this->handle.stream, this->handle.wav_data + cursor, (int)toCopy);

            cursor += toCopy;
            queued += toCopy;

            if (cursor >= this->handle.wav_data_len)
            {
                if (loop)
                {
                    cursor = 0;
                    looped.fire();
                } else
                {
                    finishedFeeding = true;
                    break;
                }
            }
        }
    }

    if (finishedFeeding && queued <= Audio::chunkSize && this->playing == true)
    {
        this->playing = false;
        finished.fire();
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