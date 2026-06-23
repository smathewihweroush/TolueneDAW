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
    static RtAudio::Api to(Toluene::Api&);
    static Toluene::Api from(RtAudio::Api&);
};
template<>
struct RtAudioMap<Toluene::AudioStreamParameters> { // for converting stream paramaters structs
    static RtAudio::StreamParameters to(Toluene::AudioStreamParameters&);
    static Toluene::AudioStreamParameters from(RtAudio::StreamParameters&);
};
template<>
struct RtAudioMap<Toluene::AudioStreamStatus> { // for converting stream status typedefs
    static RtAudioStreamStatus to(Toluene::AudioStreamStatus&);
    static Toluene::AudioStreamStatus from(RtAudioStreamStatus&);
};
template<>
struct RtAudioMap<Toluene::AudioStreamOptionFlags> {
    static RtAudioStreamFlags to(Toluene::AudioStreamOptionFlags&);
    static Toluene::AudioStreamOptionFlags from(RtAudioStreamFlags&);
};
template<>
struct RtAudioMap<Toluene::AudioStreamOptions> {
    static RtAudio::StreamOptions to(Toluene::AudioStreamOptions&);
    static Toluene::AudioStreamOptions from(RtAudio::StreamOptions&);
};
template<>
struct RtAudioMap<Toluene::SampleType> {
    static RtAudioFormat to(Toluene::SampleType&);
    static Toluene::SampleType from(RtAudioFormat&);
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
    static std::string getApiName(Toluene::Api);
    Toluene::Api getApi() override;
    void setApi(Toluene::Api) override;
    // handling of devices
    std::vector<Toluene::AudioDevice>& getAudioDevices() override;
    std::vector<unsigned int> getAudioDeviceIds() override;
    std::vector<std::string> getAudioDeviceNames() override;
    Toluene::AudioDevice* getDefaultOutputDevice() override;
    Toluene::AudioDevice* getDefaultInputDevice() override; 
    // handling of streams
    Toluene::AudioStreamId openStream(
        Toluene::AudioStreamParameters*,
        Toluene::AudioStreamParameters*,
        Toluene::SampleType,
        unsigned int,
        unsigned int*,
        Toluene::AudioCallback,
        void*,
        Toluene::AudioStreamOptions
    ) override; // this will always return 1 as the id, because there can only be one audiostream for rtaudio
    void closeStream(Toluene::AudioStreamId) override;
    Toluene::AudioStream& getStream(Toluene::AudioStreamId) override; // i dont recommend using this, but use this if you need to get an audiostream object
    bool isStreamPlaying(Toluene::AudioStreamId) override; // is stream playback active (has startedStream())?
    void startStream(Toluene::AudioStreamId) override;
    void stopStream(Toluene::AudioStreamId) override;
    // handling of class stuff
    RtAudioBackend();
    RtAudioBackend(Toluene::Api);
    ~RtAudioBackend() override;
    private:
    RtCallbackData data; // the data used in the trampoline for the rtaudiocallback
    bool mustStartedApi(); // utility
    std::unique_ptr<RtAudio> engine; // an instance of the rtaudio engine
    std::unique_ptr<Toluene::AudioStream> rtStream; // unfortunately, rtaudio supports only one stream at a time, so we only need this
    bool activeStream; // is the audiostream started?
};
