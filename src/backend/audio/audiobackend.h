#ifndef AUDIOBACKEND_H
#define AUDIOBACKEND_H

#include <string>
#include <vector>
#include <set>
#include <RtAudio.h>

using namespace std;

// TODO: add error handling functionality
// i should also probably add logging utilities or something
// just utilities in general would be useful

//
// UNIVERSAL DEFINITIONS
//
// all the stuff that's universal, required for Linesick, or just 
// required for basic functioning and linking of the backend.

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

enum SampleType {
    SINT8 = 1,
    SINT16 = 2,
    SINT24 = 4,
    SINT32 = 8,
    FLOAT32 = 16,
    FLOAT64 = 32
};

enum AudioStreamOptionFlags {
    NONINTERLEAVED = 1,
    MINIMIZE_LATENCY = 2,
    HOG_DEVICE = 4,
    REALTIME_SCHEDULING = 8,
    ALSA_USE_DEFAULT = 16
};

enum AudioStreamStatus {
    INPUT_OVERFLOW = 1,
    OUTPUT_UNDERFLOW = 2
};

typedef int (*AudioCallback)(void *outBuffer, void *inputBuffer, 
    unsigned int nFrames, double streamingTime, AudioStreamStatus status, void* userdata);

class AudioBackend;
class AudioDevice { // frankly this only considers rtaudio as a backend but idc that much :///
private: // ... a bit strange. private before public, but okay.
    AudioBackend* owner;
public:
    string deviceName;
    unsigned int deviceId;
    unsigned int outputChannels;
    unsigned int inputChannels;
    unsigned int duplexChannels;
    bool isDefaultOut;
    bool isDefaultIn;
    vector<unsigned int> supportedSampleRates;
    unsigned int currentSampleRate;
    unsigned int preferredSampleRate;
    SampleType supportedSampleTypes; // bitmask, just like in rtaudio
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
    string streamName;
    int priority;
};

class AudioStream {
public:
    bool open;
    AudioStreamParameters* outparams;
    AudioStreamParameters* inparams;
    SampleType format;
    unsigned int sampleRate;
    unsigned int* bufferSize;
    void* callback;
    void* args;
    AudioStreamOptions options;
    AudioStream(AudioStreamParameters* outparams, AudioStreamParameters* inparams, SampleType format,
        unsigned int sampleRate, unsigned int* bufferSize, void* callback, void* args, AudioStreamOptions options) : 
        outparams(outparams), inparams(inparams), sampleRate(sampleRate), bufferSize(bufferSize),
        callback(callback), args(args),
        options(options) {}
};

// TODO: make abstract audiostreamconnection class for linking 
// audiostream objects to actual library dependent audiostreams

// class for backends which handle communication with api's on their own
// should host all necessary stuff toluene requires + utilities 
// e. g. AudioBackend for RtAudio
class AudioBackend {
public:
    // handling of api
    virtual void startApi(); // start the backend communication with audio api
    virtual vector<Api> getAvailableApis(); // get api's which the backend supports
    virtual Api getApi(); // get current api
    virtual void setApi(Api); // try to change api
    // handling of devices
    virtual vector<AudioDevice> getAudioDevices(); // get a vector of all audio devices
    virtual vector<unsigned int> getAudioDeviceIds(); // get a vector of all audio device ids
    virtual vector<unsigned int> getAudioDeviceNames(); // get a vector of all audio device names
    virtual AudioDevice getDefaultOutputDevice(); // get the def out device for the system
    virtual AudioDevice getDefaultInputDevice(); // get the def out device for the system
    // handling of streams
    virtual AudioStream openStream( // essentially creates a start-able stream, ready to be used
        AudioStreamParameters*, // buffer options for outgoing stream
        AudioStreamParameters*, // buffer options for incoming stream
        SampleType, // the type individual samples are
        unsigned int, // the sample rate of the stream
        unsigned int*, // wanted buffer size of the stream. may change, so thats why its a pointer.
        AudioCallback*, // the callback which updates the stream
        void*, // user defined data
        AudioStreamOptions // special options for the stream
    );
    virtual void closeStream(AudioStream*); // delete and remove stream "safely" 

private:
    Api currentApi;
    set<AudioStream*> activeStreams; // unsure if i should use sets here really...
};

#endif