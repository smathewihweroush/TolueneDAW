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

#include <string>
#include <vector>
#include <memory>

// TODO: this is an AudioFrontend 🥀🥀🥀
// TODO: add error handling functionality
// i should also probably add logging utilities or something
// just utilities in general would be useful

// this header file contains everything related to the audio backend for toluene -
//
// the audio backend should consist of a frontend, or more specifically a class that provides everything toluene
// may need for audio playback and audio device management. this does NOT include audio manipulation.
// the AudioBackend class is the highlight, it's possiblty a singleton through which everything is handled.
// it's merely abstract though, without any definitions nor dependencies, so backend implementations inheriting from
// the AudioBackend class are required. RtAudioBackend, for example, defines all the functions, and implements them
// mainly using the RtAudio library (rtaudio.h header).
// Toluene should always use a pointer to an AudioDevice instance for audio, because big changes will be less
// if all we are calling are AudioBackend functions. actual stuff is happening in a subclass instance, acting as the AudioBackend.

namespace Toluene {
    enum Api { // underlying api used for doing audio stuff
        UNSPECIFIED, // choose first which is supported
        MACOSX_CORE, // the macosx core audio engine
        LINUX_ALSA, // linux alsa library
        UNIX_JACK, // unix jack system
        LINUX_PULSE, // linux pulseaudio library
        LINUX_OSS, // linux oss
        WINDOWS_ASIO, // windows asio
        WINDOWS_WASAPI, // windows wasapi
        WINDOWS_DS, // windows directsound
        DUMMY // nothing, no api interaction at all
    };

    // stupid wrapped typedef structs

    typedef unsigned int SampleTypeDef; // possible types for values in an audio buffer
    const SampleTypeDef SINT8 = 1; // signed 8-bit integer for the sample type
    const SampleTypeDef SINT16 = 2; // signed 16-bit integer for the sample type
    const SampleTypeDef SINT24 = 4; // signed 24-bit integer for the sample type
    const SampleTypeDef SINT32 = 8; // signed 32-bit integer for the sample type
    const SampleTypeDef FLOAT32 = 16; // 32-bit float value between 0.0-1.0 for the sample type
    const SampleTypeDef FLOAT64 = 32; // 64-bit float value between 0.0-1.0 for the sample type
    struct SampleType { // because "strict" typedefs dont exist, ive done this.
        SampleTypeDef types;
    }; // why the hell do i have to wrap this

    typedef unsigned int AudioStreamOptionFlagsDef; // bitmask of flags for audio stream options
    const AudioStreamOptionFlagsDef NONINTERLEAVED = 1;
    const AudioStreamOptionFlagsDef MINIMIZE_LATENCY = 2;
    const AudioStreamOptionFlagsDef HOG_DEVICE = 4;
    const AudioStreamOptionFlagsDef REALTIME_SCHEDULING = 8;
    const AudioStreamOptionFlagsDef ALSA_USE_DEFAULT = 16;
    struct AudioStreamOptionFlags { // because "strict" typedefs dont exist, ive done this.
        AudioStreamOptionFlagsDef flags;
    };

    typedef unsigned int AudioStreamStatusDef; // typedef for a status of an audio stream
    const AudioStreamStatusDef AUDIO_OK = 0; // nothing wrong
    const AudioStreamStatusDef INPUT_OVERFLOW = 1; // this bit is on if input overflow happens
    const AudioStreamStatusDef OUTPUT_UNDERFLOW = 2; // this bit is on if output underflow happens
    struct AudioStreamStatus { // because "strict" typedefs dont exist, ive done this.
        AudioStreamStatusDef status;
    };

    // function pointer typedef for a callback that can be used in an audio stream
    typedef int (*AudioCallback)(void *outBuffer, void *inputBuffer, 
        unsigned int nFrames, double streamingTime, AudioStreamStatus status, void* userdata);

    class AudioBackend;
    class AudioDevice { // frankly this only considers rtaudio as a backend but idc that much :///
        private: // ... a bit strange. private before public, but okay.
        AudioBackend* owner; // audio backend which controls this device
        public:
        std::string deviceName; // name of the device
        unsigned int deviceId; // actual id of the device in the underlying api
        unsigned int outputChannels; // amount of output channels
        unsigned int inputChannels; // amount of input channels
        unsigned int duplexChannels; // amount of duplex channels
        bool isDefaultIn; // bool signifying if this is the default device for providing audio input
        bool isDefaultOut; // bool signifying if this is the default device for outputting audio
        std::vector<unsigned int> supportedSampleRates; // vector containing all supported sample rates by the devide
        unsigned int currentSampleRate; // current sample rate of the device
        unsigned int preferredSampleRate; // preferred sample rate by the device
        SampleType supportedSampleTypes; // bitmask, just like in rtaudio
        unsigned int id; // id means nothing, just used to differentiate in the code.
        AudioDevice(AudioBackend* audioBackend);
    };

    struct AudioStreamParameters {
        public:
        unsigned int deviceId;
        unsigned int channelNumber;
        unsigned int channelOffset;
    };

    struct AudioStreamOptions {
        public:
        AudioStreamOptionFlags flags;
        unsigned int numberOfBuffers;
        std::string streamName;
        int priority;
    };

    typedef unsigned int AudioStreamId; // a "type" for audio stream ids
    
    class AudioStream {
        public:
        AudioStreamId id; // used to identify each audio stream
        bool started; // is playback active?
        AudioStreamParameters outparams; // output channel usage information
        AudioStreamParameters inparams; // input channel usage information
        SampleType format; // the type the audio stream will assume for each sample
        unsigned int sampleRate; // the sample rate of the stream
        unsigned int bufferSize; // the amount of samples in one callback for one channel (does this change?)
        void* callback; // a pointer to the function being used for the callback
        void* args; // pointer to additional stuff provided graciously from an openStream function call
        AudioStreamOptions options; // miscellaneous options for an audio stream
        AudioStream(AudioBackend* backend, 
            AudioStreamParameters outparams, AudioStreamParameters inparams, SampleType format,
            unsigned int sampleRate, unsigned int bufferSize, void* callback, void* args, AudioStreamOptions options) : 
            outparams(outparams), inparams(inparams), sampleRate(sampleRate), bufferSize(bufferSize),
            callback(callback), args(args), options(options), backend(backend) {}
        AudioStream(AudioBackend* backend) : 
            backend(backend) {}
        private:
        AudioBackend* backend; // audiobackend instance overseeing this stream
    };

    // class for backends which handle communication with api's on their own
    // should host all necessary stuff toluene requires + utilities 
    // e. g. AudioBackend for RtAudio
    class AudioBackend {
        public:
        // handling of api
        virtual void startApi() = 0; // start the backend communication with audio api
        static std::vector<Api> getAvailableApis(); // get api's which the backend supports
        static std::string getApiName(Toluene::Api api); // get the name of a toluene api
        virtual Api getApi() = 0; // get current api
        virtual void setApi(Api api) = 0; // try to change api
        // handling of devices
        virtual std::vector<AudioDevice>& getAudioDevices() = 0; // get a vector of all audio devices
        virtual std::vector<unsigned int> getAudioDeviceIds() = 0; // get a vector of all audio device ids
        virtual std::vector<std::string> getAudioDeviceNames() = 0; // get a vector of all audio device names
        virtual AudioDevice* getDefaultOutputDevice() = 0; // get the def out device for the system
        virtual AudioDevice* getDefaultInputDevice() = 0; // get the def out device for the system
        // handling of streams
        virtual AudioStreamId openStream( // essentially creates a start-able stream, ready to be used
            AudioStreamParameters* outputBuffer, // buffer options for outgoing stream
            AudioStreamParameters* inputBuffer, // buffer options for incoming stream
            SampleType type, // the type individual samples are
            unsigned int sampleRate, // the sample rate of the stream
            unsigned int* bufferSize, // wanted buffer size of the stream. may change, so thats why its a pointer.
            AudioCallback callback, // the callback which updates the stream
            void* userdata = NULL, // user defined data
            AudioStreamOptions options = {} // special options for the stream
                                    // TODO: what should be here for the default?
        ) = 0;
        virtual void closeStream(AudioStreamId streamId) = 0; // delete and remove stream "safely"
        virtual AudioStream& getStream(AudioStreamId streamId) = 0; // i dont recommend using this, but use this if you need to get an audiostream object
        virtual bool isStreamPlaying(AudioStreamId streamId) = 0; // is stream playback active (has startedStream())?
        virtual void startStream(AudioStreamId streamId) = 0; // start stream playback
        virtual void stopStream(AudioStreamId streamId) = 0; // stop stream playback
        // handling of class stuff
        AudioBackend();
        AudioBackend(Api api);
        virtual ~AudioBackend() = default;

        Api currentApi; // the api that is being, or will be used the next time startApi() is called
        bool startedApi; // has the api been started?
        protected:
        std::vector<std::unique_ptr<AudioStream>> activeStreams; // unsure if i should use a vector here really...
        std::vector<AudioDevice> devices; // data of all available devices
    };
}