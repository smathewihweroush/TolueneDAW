#include <audiobackend.h>
#include <iostream>
#include <memory>
#include <rtaudiobackend.h>

RtAudio::Api fromApiEnum(Toluene::Api api) {
    switch (api) {
        case Toluene::UNSPECIFIED: return RtAudio::UNSPECIFIED;
        case Toluene::MACOSX_CORE: return RtAudio::MACOSX_CORE;
        case Toluene::LINUX_ALSA: return RtAudio::LINUX_ALSA;
        case Toluene::UNIX_JACK: return RtAudio::UNIX_JACK;
        case Toluene::LINUX_PULSE: return RtAudio::LINUX_PULSE;
        case Toluene::LINUX_OSS: return RtAudio::LINUX_OSS;
        case Toluene::WINDOWS_ASIO: return RtAudio::WINDOWS_ASIO;
        case Toluene::WINDOWS_WASAPI: return RtAudio::WINDOWS_WASAPI;
        case Toluene::WINDOWS_DS: return RtAudio::WINDOWS_DS;
        case Toluene::DUMMY: return RtAudio::RTAUDIO_DUMMY;
    }
    std::cerr << "Toluene Api could not be converted to an RtAudio one. Returning UNSPECIFIED Api instead.\n";
    return RtAudio::UNSPECIFIED; // edge case
};

Toluene::Api toApiEnum(RtAudio::Api api) {
    switch (api) {
        case RtAudio::UNSPECIFIED: return Toluene::UNSPECIFIED;
        case RtAudio::MACOSX_CORE: return Toluene::MACOSX_CORE;
        case RtAudio::LINUX_ALSA: return Toluene::LINUX_ALSA;
        case RtAudio::UNIX_JACK: return Toluene::UNIX_JACK;
        case RtAudio::LINUX_PULSE: return Toluene::LINUX_PULSE;
        case RtAudio::LINUX_OSS: return Toluene::LINUX_OSS;
        case RtAudio::WINDOWS_ASIO: return Toluene::WINDOWS_ASIO;
        case RtAudio::WINDOWS_WASAPI: return Toluene::WINDOWS_WASAPI;
        case RtAudio::WINDOWS_DS: return Toluene::WINDOWS_DS;
        case RtAudio::RTAUDIO_DUMMY: return Toluene::DUMMY;
        case RtAudio::NUM_APIS:
            std::cerr << "Attempted to convert NUM_APIS to Toluene Api. Returning UNSPECIFIED Api instead.\n";
            return Toluene::UNSPECIFIED;
    }
    std::cerr << "RtAudio Api could not be converted to a Toluene one. Returning UNSPECIFIED Api instead.\n";
    return Toluene::UNSPECIFIED; // edge case
};

void RtAudioBackend::startApi() {
    if (startedApi) {
        std::cerr << "RtAudio instance with Api already created.\n";
        return;
    }
    engine = std::make_unique<RtAudio>(RtAudio(fromApiEnum(currentApi))); // TODO: no error callback!!! 333:
    // TODO: not just hope it initialized right 
    startedApi = true;
}

std::vector<Toluene::Api> RtAudioBackend::getAvailableApis() {
    std::vector<RtAudio::Api> apis;
    std::vector<Toluene::Api> tolueneApis;
    RtAudio::getCompiledApi(apis);
    // TODO: not just hope it got compiled apis right
    tolueneApis.reserve(apis.size() + 1); // just in case +1!
    for (int i = 0; i < apis.size(); i++) {
        tolueneApis.push_back(toApiEnum(apis[i]));
    }
    return tolueneApis;
};

std::string RtAudioBackend::getApiName(Toluene::Api api) {
    switch (api) {
        case Toluene::UNSPECIFIED: return "Automatic";
        case Toluene::MACOSX_CORE: return "MacOSX Core";
        case Toluene::LINUX_ALSA: return "Linux ALSA";
        case Toluene::UNIX_JACK: return "Unix Jack";
        case Toluene::LINUX_PULSE: return "Linux PulseAudio";
        case Toluene::LINUX_OSS: return "Linux OSS";
        case Toluene::WINDOWS_ASIO: return "Windows ASIO";
        case Toluene::WINDOWS_WASAPI: return "Windows WASAPI";
        case Toluene::WINDOWS_DS: return "Windows DirectSound";
        case Toluene::DUMMY: return "Dummy";
    }
    std::cerr << "Could not find Api name.\n";
    return "Unknown"; // edge case
};

Toluene::Api RtAudioBackend::getApi() { // this is such a pointless function im(nsh)o
    return currentApi;
};

void RtAudioBackend::setApi(Toluene::Api api) {
    if (startedApi) {
        std::cerr << "RtAudio instance has already been started with Api. Cannot change Api.\n";
        return;
    }
    currentApi = api;
};

std::vector<Toluene::AudioDevice*> RtAudioBackend::getAudioDevices() {
    if (!startedApi) {
        std::cerr << "RtAudio instance hasn't been started.\n";
        return std::vector<Toluene::AudioDevice*>();
    }
    std::vector<unsigned int> ids = engine->getDeviceIds();
    for (int i = 0; i < ids.size(); i++) {
        RtAudio::DeviceInfo info = engine->getDeviceInfo(ids[i]);
        std::cout << " > " << info.name << std::endl;
    }
    return std::vector<Toluene::AudioDevice*>(); // TODO: stub.
};

std::vector<unsigned int> RtAudioBackend::getAudioDeviceIds() { return std::vector<unsigned int>(); }; // TODO: stub.
std::vector<unsigned int> RtAudioBackend::getAudioDeviceNames() { return std::vector<unsigned int>(); }; // TODO: stub.
Toluene::AudioDevice* RtAudioBackend::getDefaultOutputDevice() { return (Toluene::AudioDevice*)(new Toluene::AudioDevice()); }; // TODO: stub.
Toluene::AudioDevice* RtAudioBackend::getDefaultInputDevice() { return (Toluene::AudioDevice*)(new Toluene::AudioDevice()); }; // TODO: stub.
// handling of streams
Toluene::AudioStream RtAudioBackend::openStream(
    Toluene::AudioStreamParameters*,
    Toluene::AudioStreamParameters*,
    Toluene::SampleType,
    unsigned int,
    unsigned int*,
    Toluene::AudioCallback*,
    void*,
    Toluene::AudioStreamOptions
) { return *new Toluene::AudioStream(); }; // TODO: stub.
void RtAudioBackend::closeStream(Toluene::AudioStream*) {} ; // TODO: stub.


RtAudioBackend::RtAudioBackend(Toluene::Api api) : Toluene::AudioBackend(api) {
    currentApi = api;
    startedApi = false;
}
RtAudioBackend::~RtAudioBackend() {
    
} // TODO: stub.