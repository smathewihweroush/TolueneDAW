/*
Toluene: digital audio workspace with a text user interface
Copyright 2026 smathewih

This file is part of Toluene.

Toluene is free software: you can redistribute it and/or modify it under 
the terms of the GNU General Public License as published by the Free 
Software Foundation, either version 3 of the License, or (at your option) 
any later version.

Toluene is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with 
Toluene. If not, see <https://www.gnu.org/licenses/>. 
*/


#pragma once

#include <audiobackend.h>
#include <RtAudio.h>
#include <memory>
#include <string>

// the RtAudioBackend adds an implementation of the AudioBackend utilizing RtAudio in the back

template<typename T>
struct RtAudioMap;

template<>
struct RtAudioMap<Toluene::Api> { // for converting api types
    static RtAudio::Api to(Toluene::Api& tolueneApi);
    static Toluene::Api from(RtAudio::Api& rtaudioApi);
};
template<>
struct RtAudioMap<Toluene::AudioStreamParameters> { // for converting stream paramaters structs
    static RtAudio::StreamParameters to(Toluene::AudioStreamParameters& tolueneParams);
    static Toluene::AudioStreamParameters from(RtAudio::StreamParameters& rtaudioParams);
};
template<>
struct RtAudioMap<Toluene::AudioStreamStatus> { // for converting stream status typedefs
    static RtAudioStreamStatus to(Toluene::AudioStreamStatus& tolueneStreamStatus);
    static Toluene::AudioStreamStatus from(RtAudioStreamStatus& rtaudioStreamStatus);
};
template<>
struct RtAudioMap<Toluene::AudioStreamOptionFlags> {
    static RtAudioStreamFlags to(Toluene::AudioStreamOptionFlags& tolueneFlags);
    static Toluene::AudioStreamOptionFlags from(RtAudioStreamFlags& rtaudioFlags);
};
template<>
struct RtAudioMap<Toluene::AudioStreamOptions> {
    static RtAudio::StreamOptions to(Toluene::AudioStreamOptions& tolueneStreamOptions);
    static Toluene::AudioStreamOptions from(RtAudio::StreamOptions& rtaudioStreamOptions);
};
template<>
struct RtAudioMap<Toluene::SampleType> {
    static RtAudioFormat to(Toluene::SampleType& tolueneType);
    static Toluene::SampleType from(RtAudioFormat& rtaudioType);
};

struct RtCallbackData {
    Toluene::AudioCallback callback;
    void* userData;
};

class RtAudioBackend : public Toluene::AudioBackend {
    public:
    // handling of api
    void startApi() override;
    static std::vector<Toluene::Api> getAvailableApis();
    static std::string getApiName(Toluene::Api api);
    Toluene::Api getApi() override;
    void setApi(Toluene::Api api) override;
    // handling of devices
    std::vector<Toluene::AudioDevice>& getAudioDevices() override;
    std::vector<unsigned int> getAudioDeviceIds() override;
    std::vector<std::string> getAudioDeviceNames() override;
    Toluene::AudioDevice* getDefaultOutputDevice() override;
    Toluene::AudioDevice* getDefaultInputDevice() override; 
    // handling of streams
    Toluene::AudioStreamId openStream(
        Toluene::AudioStreamParameters* outputBuffer,
        Toluene::AudioStreamParameters* inputBuffer,
        Toluene::SampleType type,
        unsigned int sampleRate,
        unsigned int* bufferSize,
        Toluene::AudioCallback callback,
        void* userdata,
        Toluene::AudioStreamOptions options
    ) override; // this will always return 1 as the id, because there can only be one audiostream for rtaudio
    void closeStream(Toluene::AudioStreamId streamId) override;
    Toluene::AudioStream& getStream(Toluene::AudioStreamId streamId) override; // i dont recommend using this, but use this if you need to get an audiostream object
    bool isStreamPlaying(Toluene::AudioStreamId streamId) override; // is stream playback active (has startedStream())?
    void startStream(Toluene::AudioStreamId streamId) override;
    void stopStream(Toluene::AudioStreamId streamId) override;
    // handling of class stuff
    RtAudioBackend();
    RtAudioBackend(Toluene::Api api);
    ~RtAudioBackend() override;
    private:
    RtCallbackData data; // the data used in the trampoline for the rtaudiocallback
    bool mustStartedApi(); // utility
    std::unique_ptr<RtAudio> engine; // an instance of the rtaudio engine
    std::unique_ptr<Toluene::AudioStream> rtStream; // unfortunately, rtaudio supports only one stream at a time, so we only need this
    bool activeStream; // is the audiostream started?
};
