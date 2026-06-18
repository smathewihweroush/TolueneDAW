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
    static RtAudio::Api to(Toluene::Api);
    static Toluene::Api from(RtAudio::Api);
};
template<>
struct RtAudioMap<Toluene::AudioStreamParameters> { // for converting stream paramaters structs
    static RtAudio::StreamParameters to(Toluene::AudioStreamParameters);
    static Toluene::AudioStreamParameters from(RtAudio::StreamParameters);
};
template<>
struct RtAudioMap<Toluene::AudioStreamStatus> { // for converting stream status typedefs
    static RtAudioStreamStatus to(Toluene::AudioStreamStatus);
    static Toluene::AudioStreamStatus from(RtAudioStreamStatus);
};
template<>
struct RtAudioMap<Toluene::AudioStreamOptionFlags> {
    static RtAudioStreamFlags to(Toluene::AudioStreamOptionFlags);
    static Toluene::AudioStreamOptionFlags from(RtAudioStreamFlags);
};
template<>
struct RtAudioMap<Toluene::AudioStreamOptions> {
    static RtAudio::StreamOptions to(Toluene::AudioStreamOptions);
    static Toluene::AudioStreamOptions from(RtAudio::StreamOptions);
};
template<>
struct RtAudioMap<Toluene::SampleType> {
    static RtAudioFormat to(Toluene::SampleType);
    static Toluene::SampleType from(RtAudioFormat);
};

struct RtCallbackData {
    Toluene::AudioCallback* callback;
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
    std::vector<Toluene::AudioDevice*> getAudioDevices() override;
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
        Toluene::AudioCallback*,
        void*,
        Toluene::AudioStreamOptions
    ) override; // this will always return 1 as the id, because there can only be one audiostream for rtaudio
    void closeStream(Toluene::AudioStreamId) override;
    // handling of class stuff
    RtAudioBackend(Toluene::Api);
    ~RtAudioBackend() override;
    private:
    bool mustStartedApi(); // utility
    std::unique_ptr<RtAudio> engine; // an instance of the rtaudio engine
    std::unique_ptr<Toluene::AudioStream> rtStream; // unfortunately, rtaudio supports only one stream at a time, so we only need this
    bool activeStream;
};
