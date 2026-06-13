#pragma once

#include <audiobackend.h>
#include <RtAudio.h>
#include <memory>

// the RtAudioBackend adds an implementation of the AudioBackend utilizing RtAudio in the back

RtAudio::Api fromApiEnum(Toluene::Api); // attempts conversion from Toluene::Api to RtAudio equivalent
Toluene::Api toApiEnum(RtAudio::Api); // attempts conversion from RtAudio::Api to Toluene equivalent

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
    std::vector<unsigned int> getAudioDeviceNames() override;
    Toluene::AudioDevice* getDefaultOutputDevice() override;
    Toluene::AudioDevice* getDefaultInputDevice() override; 
    // handling of streams
    Toluene::AudioStream openStream(
        Toluene::AudioStreamParameters*,
        Toluene::AudioStreamParameters*,
        Toluene::SampleType,
        unsigned int,
        unsigned int*,
        Toluene::AudioCallback*,
        void*,
        Toluene::AudioStreamOptions
    ) override;
    void closeStream(Toluene::AudioStream*) override;
    // handling of class stuff
    RtAudioBackend(Toluene::Api);
    ~RtAudioBackend() override;
private:
    std::unique_ptr<RtAudio> engine; // an instance of the rtaudio engine
};
