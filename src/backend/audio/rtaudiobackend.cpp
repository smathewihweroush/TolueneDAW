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


#include <audiobackend.h>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <rtaudiobackend.h>
#include <string>
#include <vector>

// casts, as a part of RtAudioMap<> template classes

RtAudio::Api RtAudioMap<Toluene::Api>::to(Toluene::Api api) {
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
        default:
            std::cerr << "Unknown RtAudio Api. Returning UNSPECIFIED Api instead.\n";
            return RtAudio::UNSPECIFIED; // edge case
    }   
}
Toluene::Api RtAudioMap<Toluene::Api>::from(RtAudio::Api api) {
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
            std::cerr << "Attempted to convert NUM_APIS to Toluene Api, check your code. Returning UNSPECIFIED Api instead.\n";
            return Toluene::UNSPECIFIED;
        default:
            std::cerr << "RtAudio Api could not be converted to a Toluene one. Returning UNSPECIFIED Api instead.\n";
            return Toluene::UNSPECIFIED; // edge case
    }
}

RtAudio::StreamParameters RtAudioMap<Toluene::AudioStreamParameters>::to(Toluene::AudioStreamParameters params) {
    RtAudio::StreamParameters pars{params.deviceId, params.channelNumber, params.channelOffset};
    return pars;
}
Toluene::AudioStreamParameters RtAudioMap<Toluene::AudioStreamParameters>::from(RtAudio::StreamParameters params) {
    Toluene::AudioStreamParameters pars{params.deviceId, params.nChannels, params.firstChannel};
    return pars;
}

RtAudioStreamStatus RtAudioMap<Toluene::AudioStreamStatus>::to(Toluene::AudioStreamStatus params) {
    return params.status;
}
Toluene::AudioStreamStatus RtAudioMap<Toluene::AudioStreamStatus>::from(RtAudioStreamStatus status) {
    return {status};
}

RtAudioStreamFlags RtAudioMap<Toluene::AudioStreamOptionFlags>::to(Toluene::AudioStreamOptionFlags flags) {
    return flags.flags;
}   
Toluene::AudioStreamOptionFlags RtAudioMap<Toluene::AudioStreamOptionFlags>::from(RtAudioStreamFlags flags) {
    return {flags};
}   

RtAudio::StreamOptions RtAudioMap<Toluene::AudioStreamOptions>::to(Toluene::AudioStreamOptions streamOptions) {
    RtAudio::StreamOptions strops{ RtAudioMap<Toluene::AudioStreamOptionFlags>::to(streamOptions.flags), 
        streamOptions.numberOfBuffers, streamOptions.streamName, 
        streamOptions.priority};
    return strops;
}
Toluene::AudioStreamOptions RtAudioMap<Toluene::AudioStreamOptions>::from(RtAudio::StreamOptions streamOptions) {
    Toluene::AudioStreamOptions strops{RtAudioMap<Toluene::AudioStreamOptionFlags>::from(streamOptions.flags),
        streamOptions.numberOfBuffers, streamOptions.streamName, 
        streamOptions.priority};
    return strops;
}

RtAudioFormat RtAudioMap<Toluene::SampleType>::to(Toluene::SampleType sampleType) {
    return sampleType.types;
}

Toluene::SampleType RtAudioMap<Toluene::SampleType>::from(RtAudioFormat format) {
    return {static_cast<Toluene::SampleTypeDef>(format)};
}

// utilities

bool RtAudioBackend::mustStartedApi() {
    if (!startedApi) {
        std::cerr << "RtAudio instance hasn't been started.\n";
        return true;
    }
    return false;
}

// api related

void RtAudioBackend::startApi() {
    if (startedApi) {
        std::cerr << "RtAudio instance with Api already created.\n";
        return;
    }
    engine = std::make_unique<RtAudio>(RtAudioMap<Toluene::Api>::to(currentApi)); // TODO: no error callback!!! 333:
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
        tolueneApis.push_back(RtAudioMap<Toluene::Api>::from(apis[i]));
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

// audio device related

bool testAudioDevicesData(RtAudio::DeviceInfo& rtDevice, Toluene::AudioDevice& tolueneDevice) {
    // utility to either check if two devices are same, or to equal them out
    if (rtDevice.isDefaultInput == tolueneDevice.isDefaultIn &&
        rtDevice.isDefaultOutput == tolueneDevice.isDefaultOut &&
        rtDevice.inputChannels == tolueneDevice.inputChannels &&
        rtDevice.outputChannels == tolueneDevice.outputChannels &&
        rtDevice.duplexChannels == tolueneDevice.duplexChannels &&
        rtDevice.currentSampleRate == tolueneDevice.currentSampleRate &&
        rtDevice.preferredSampleRate == tolueneDevice.preferredSampleRate &&
        rtDevice.name == tolueneDevice.deviceName &&
        rtDevice.nativeFormats == tolueneDevice.supportedSampleTypes.types &&
        rtDevice.sampleRates == tolueneDevice.supportedSampleRates) return true;
    tolueneDevice.deviceName = rtDevice.name;
    tolueneDevice.deviceId = rtDevice.ID;
    tolueneDevice.outputChannels = rtDevice.outputChannels;
    tolueneDevice.inputChannels = rtDevice.inputChannels;
    tolueneDevice.duplexChannels = rtDevice.duplexChannels;
    tolueneDevice.supportedSampleRates = rtDevice.sampleRates;
    tolueneDevice.currentSampleRate = rtDevice.currentSampleRate;
    tolueneDevice.preferredSampleRate = rtDevice.preferredSampleRate;
    tolueneDevice.supportedSampleTypes.types = rtDevice.nativeFormats;
    // TODO: there has to be some better way to do this.
    return false;
}

std::vector<Toluene::AudioDevice*> RtAudioBackend::getAudioDevices() { // this monstrosity is O(n^2). yikes...
    if (mustStartedApi()) return std::vector<Toluene::AudioDevice*>();
    std::vector<unsigned int> ids = engine->getDeviceIds();
    std::vector<bool> checked(devices.size(), 0);
    int prevsiz = devices.size();
    for (int i = 0; i < ids.size(); i++) {
        RtAudio::DeviceInfo info = engine->getDeviceInfo(ids[i]);
        Toluene::AudioDevice* match = nullptr;
        for (int j = 0; j < devices.size(); j++) {
            if (devices[j].id == info.ID) {
                match = &devices[j];
                checked[j] = 1;
                break;
            }
        }
        if (match != nullptr) {
            if (!testAudioDevicesData(info, *match)) {
                std::cerr << "RtAudio device with same Id as Toluene AudioDevice don't have same identical information suddenly.\n";
            }
        } else {
            Toluene::AudioDevice newDevice(this);
            newDevice.deviceName = info.name;
            newDevice.deviceId = info.ID;
            newDevice.outputChannels = info.outputChannels;
            newDevice.inputChannels = info.inputChannels;
            newDevice.duplexChannels = info.duplexChannels;
            newDevice.supportedSampleRates = info.sampleRates;
            newDevice.currentSampleRate = info.currentSampleRate;
            newDevice.preferredSampleRate = info.preferredSampleRate;
            newDevice.supportedSampleTypes.types = info.nativeFormats;
            // there has to be some better way to do this.
            devices.push_back(newDevice);
        }
    }
    for (int i = prevsiz - 1; i >= 0; i--) {
        if (checked[i] == 0) {
            devices.erase(std::next(devices.begin(), i));
        }
    }
    return std::vector<Toluene::AudioDevice*>(); // TODO: this sucks frankly AND STUB
};

std::vector<unsigned int> RtAudioBackend::getAudioDeviceIds() {
    if (mustStartedApi()) return std::vector<unsigned int>();
    getAudioDevices();
    std::vector<unsigned int> ids;
    ids.reserve(devices.size());
    for (int i = 0; i < devices.size(); i++) {
        ids.push_back(devices[i].id);
    }
    return ids;
};
std::vector<std::string> RtAudioBackend::getAudioDeviceNames() {
    if (mustStartedApi()) return std::vector<std::string>();
    getAudioDevices();
    std::vector<std::string> names;
    names.reserve(devices.size());
    for (int i = 0; i < devices.size(); i++) {
        names.push_back(devices[i].deviceName);
    }
    return names;
};
Toluene::AudioDevice* RtAudioBackend::getDefaultOutputDevice() { 
    if (mustStartedApi()) return nullptr;
    getAudioDevices();
    Toluene::AudioDevice* found = NULL;
    for (int i = 0; i < devices.size(); i++) {
        if (devices[i].isDefaultOut) found = &devices[i];
    }
    if (found == NULL) {
        std::cerr << "Error: Didn't find any default output device.\n";
    }
    return found;
};
Toluene::AudioDevice* RtAudioBackend::getDefaultInputDevice() { // all of these functions are O(N)...
                                                                // not including the comlexity of getAudioDevices().
    if (mustStartedApi()) return nullptr;
    getAudioDevices();
    Toluene::AudioDevice* found;
    for (int i = 0; i < devices.size(); i++) {
        if (devices[i].isDefaultIn) found = &devices[i];
    }
    if (found == NULL) {
        std::cerr << "Error: Didn't find any default input device.\n";
    }
    return found;
};

// handling of streams

int trampoline(void *outputBuffer, void *inputBuffer, unsigned int nFrames, 
    double streamTime, RtAudioStreamStatus status, void *userData) {
    auto data = static_cast<RtCallbackData*>(userData);
    return data->callback( // TODO: is this okay?
        outputBuffer, inputBuffer, nFrames, streamTime, 
        RtAudioMap<Toluene::AudioStreamStatus>::from(status), data->userData
    );
} // used in the below function, rtaudiocallback which sets up a call to actual toluene::audiocallback

RtCallbackData data; // TODO: these variables are a heap mess. change this!!!
RtAudio::StreamParameters* outpms;
RtAudio::StreamParameters* inpms;
RtAudio::StreamParameters oup;
RtAudio::StreamParameters inp;
RtAudio::StreamOptions strops;
Toluene::AudioStreamId RtAudioBackend::openStream(
    Toluene::AudioStreamParameters* outparams,
    Toluene::AudioStreamParameters* inparams,
    Toluene::SampleType type,
    unsigned int sampleRate,
    unsigned int* bufferSize,
    Toluene::AudioCallback callback,
    void* args,
    Toluene::AudioStreamOptions options) {
    if (activeStream) {
        std::cerr << "Already has an open stream. RtAudio supports only one concurrent stream. Closing current stream.\n";
    }
    data.callback = callback;
    data.userData = args;
    outpms = NULL;
    if (outparams != NULL) {
        oup = RtAudioMap<Toluene::AudioStreamParameters>::to(*outparams);
        outpms = &oup;
    }
    inpms = NULL;
    if (inparams != NULL) {
        inp = RtAudioMap<Toluene::AudioStreamParameters>::to(*inparams);
        inpms = &inp;
    }
    strops = RtAudioMap<Toluene::AudioStreamOptions>::to(options);
    engine->openStream(outpms, inpms, 
        RtAudioMap<Toluene::SampleType>::to(type), sampleRate, bufferSize, 
        &trampoline, &data, &strops);
        if (!engine->isStreamOpen()) {
            std::cerr << "Error: Could not open RtAudio stream.\n";
            return 0;
        }
    activeStream = 1; // we can no say we did it!
    rtStream = std::make_unique<Toluene::AudioStream>(this, outparams, inparams, type, 
        sampleRate, bufferSize, &callback, args, options);
    return 1;
};

Toluene::AudioStream& RtAudioBackend::getStream(Toluene::AudioStreamId id) {
    if (id != 1) {
        throw std::runtime_error("Invalid AudioStreamId for RtAudioBackend."); // this is the first time i had to use this.
        // i really, really dont want to deal with returning an invalid reference here.
    } else if (!activeStream) {
        throw std::runtime_error("No streams have been started."); // same here
    }
    return *rtStream; // TODO: does this actually work?
}

bool RtAudioBackend::isStreamPlaying(Toluene::AudioStreamId id) {
    if (id != 1) {
        std::cerr << "Invalid AudioStreamId for RtAudioBackend.\n";
        return false;
    } else if (!activeStream) {
        std::cerr << "No streams are open.\n";
        return false;
    }
    return rtStream->started;
}

void RtAudioBackend::closeStream(Toluene::AudioStreamId id) {
    if (id != 1) {
        std::cerr << "Invalid AudioStreamId for RtAudioBackend.\n";
        return;
    } else if (!activeStream) {
        std::cerr << "No streams are open.\n";
        return;
    }
    if (isStreamPlaying(id)) {
        stopStream(id);
    }
    engine->closeStream();
    if (engine->isStreamOpen()) {
        std::cerr << "Error: Could not close RtAudio stream. Somehow.\n";
        return;
    }
    activeStream = 0;
}

void RtAudioBackend::startStream(Toluene::AudioStreamId id) {
    if (id != 1) {
        std::cerr << "Invalid AudioStreamId for RtAudioBackend.\n";
        return;
    } else if (!activeStream) {
        std::cerr << "No streams are open.\n";
        return;
    }
    if (rtStream->started == true) {
        std::cerr << "Stream already started.\n";
        return;
    }
    engine->startStream(); // TODO: gosh this is just assuming it works flawlessly, all of these are
    rtStream->started = true;
}

void RtAudioBackend::stopStream(Toluene::AudioStreamId id) {
    if (id != 1) {
        std::cerr << "Invalid AudioStreamId for RtAudioBackend.\n";
        return;
    } else if (!activeStream) {
        std::cerr << "No streams are open.\n";
        return;
    }
    if (rtStream->started == false) {
        std::cerr << "Stream has not been started yet.\n";
        return;
    }
    engine->stopStream();
    rtStream->started = false;
}

// rtaudiobackend stuff (constructor/deconstructor)

RtAudioBackend::RtAudioBackend(Toluene::Api api) : Toluene::AudioBackend(api) {
    currentApi = api;
    startedApi = false;
}

RtAudioBackend::~RtAudioBackend() {
    
} // TODO: stub.