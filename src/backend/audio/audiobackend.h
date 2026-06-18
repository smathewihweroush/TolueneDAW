#pragma once

#include <string>
#include <vector>
#include <RtAudio.h>

// TODO: add error handling functionality
// i should also probably add logging utilities or something
// just utilities in general would be useful

//
// UNIVERSAL DEFINITIONS
//
// all the stuff that's universal, required for Linesick, or just 
// required for basic functioning and linking of the backend.

namespace Toluene {
    enum Api {
        UNSPECIFIED,
        MACOSX_CORE,
        LINUX_ALSA,
        UNIX_JACK,
        LINUX_PULSE,
        LINUX_OSS,
        WINDOWS_ASIO,
        WINDOWS_WASAPI,
        WINDOWS_DS,
        DUMMY
    };

    // stupid wrapped typedef structs

    typedef int SampleTypeDef;
    const SampleTypeDef SINT8 = 1;
    const SampleTypeDef SINT16 = 2;
    const SampleTypeDef SINT24 = 4;
    const SampleTypeDef SINT32 = 8;
    const SampleTypeDef FLOAT32 = 16;
    const SampleTypeDef FLOAT64 = 32;
    struct SampleType {
        SampleTypeDef types;
    }; // why the hell do i have to wrap this

    typedef unsigned int AudioStreamOptionFlagsDef;
    const AudioStreamOptionFlagsDef NONINTERLEAVED = 1;
    const AudioStreamOptionFlagsDef MINIMIZE_LATENCY = 2;
    const AudioStreamOptionFlagsDef HOG_DEVICE = 4;
    const AudioStreamOptionFlagsDef REALTIME_SCHEDULING = 8;
    const AudioStreamOptionFlagsDef ALSA_USE_DEFAULT = 16;
    struct AudioStreamOptionFlags {
        AudioStreamOptionFlagsDef flags;
    };

    typedef unsigned int AudioStreamStatusDef;
    const AudioStreamStatusDef OK = 0;
    const AudioStreamStatusDef INPUT_OVERFLOW = 1;
    const AudioStreamStatusDef OUTPUT_UNDERFLOW = 2;
    struct AudioStreamStatus {
        AudioStreamStatusDef status;
    };

    typedef int (*AudioCallback)(void *outBuffer, void *inputBuffer, 
        unsigned int nFrames, double streamingTime, AudioStreamStatus status, void* userdata);

    class AudioBackend;
    class AudioDevice { // frankly this only considers rtaudio as a backend but idc that much :///
        private: // ... a bit strange. private before public, but okay.
        AudioBackend* owner;
        public:
        std::string deviceName;
        unsigned int deviceId;
        unsigned int outputChannels;
        unsigned int inputChannels;
        unsigned int duplexChannels;
        bool isDefaultIn;
        bool isDefaultOut;
        std::vector<unsigned int> supportedSampleRates;
        unsigned int currentSampleRate;
        unsigned int preferredSampleRate;
        SampleType supportedSampleTypes; // bitmask, just like in rtaudio
        unsigned int id; // id means nothing, just used to differentiate in the code.
        AudioDevice(AudioBackend*);
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

    typedef unsigned int AudioStreamId;
    
    class AudioStream {
        public:
        AudioStreamId id;
        bool started;
        AudioStreamParameters* outparams;
        AudioStreamParameters* inparams;
        SampleType format;
        unsigned int sampleRate;
        unsigned int* bufferSize;
        void* callback;
        void* args;
        AudioStreamOptions options;
        AudioStream(AudioBackend* backend, 
            AudioStreamParameters* outparams, AudioStreamParameters* inparams, SampleType format,
            unsigned int sampleRate, unsigned int* bufferSize, void* callback, void* args, AudioStreamOptions options) : 
            outparams(outparams), inparams(inparams), sampleRate(sampleRate), bufferSize(bufferSize),
            callback(callback), args(args), options(options), backend(backend) {}
        AudioStream(AudioBackend* backend) : 
            backend(backend) {}
        void startStream();
        void stopStream();
        private:
        AudioBackend* backend;
    };

    // TODO: make abstract audiostreamconnection class for linking 
    // audiostream objects to actual library dependent audiostreams

    // class for backends which handle communication with api's on their own
    // should host all necessary stuff toluene requires + utilities 
    // e. g. AudioBackend for RtAudio
    class AudioBackend {
        public:
        // handling of api
        virtual void startApi() = 0; // start the backend communication with audio api
        static std::vector<Api> getAvailableApis(); // get api's which the backend supports
        static std::string getApiName(Toluene::Api);
        virtual Api getApi() = 0; // get current api
        virtual void setApi(Api) = 0; // try to change api
        // handling of devices
        virtual std::vector<AudioDevice*> getAudioDevices() = 0; // get a vector of all audio devices
        virtual std::vector<unsigned int> getAudioDeviceIds() = 0; // get a vector of all audio device ids
        virtual std::vector<std::string> getAudioDeviceNames() = 0; // get a vector of all audio device names
        virtual AudioDevice* getDefaultOutputDevice() = 0; // get the def out device for the system
        virtual AudioDevice* getDefaultInputDevice() = 0; // get the def out device for the system
        // handling of streams
        virtual AudioStreamId openStream( // essentially creates a start-able stream, ready to be used
            AudioStreamParameters*, // buffer options for outgoing stream
            AudioStreamParameters*, // buffer options for incoming stream
            SampleType, // the type individual samples are
            unsigned int, // the sample rate of the stream
            unsigned int*, // wanted buffer size of the stream. may change, so thats why its a pointer.
            AudioCallback*, // the callback which updates the stream
            void*, // user defined data
            AudioStreamOptions // special options for the stream
        ) = 0;
        virtual void closeStream(AudioStreamId) = 0; // delete and remove stream "safely"
        // handling of class stuff
        AudioBackend(Api);
        virtual ~AudioBackend() = default;

        Api currentApi;
        bool startedApi;
        protected:
        std::vector<std::unique_ptr<AudioStream>> activeStreams; // unsure if i should use a vector here really...
        std::vector<AudioDevice> devices;
        // TODO: see if unique_ptr/shared_ptr may be better instead of pointers
    };
}