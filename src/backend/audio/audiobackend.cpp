#include "audiobackend.h"

using namespace std;

/*Api AudioBackend::getApi() { // get current api
        return currentApi;
    }
void AudioBackend::setApi(Api newApi) { // try to change api
    currentApi = newApi;
}
AudioStream AudioBackend::openStream(
        AudioStreamParameters* outparams, 
        AudioStreamParameters* inparams, 
        SampleType format,
        unsigned int sampleRate,
        unsigned int* bufferSize,
        AudioCallback* callback,
        void* args,
        AudioStreamOptions options
    ) {
    AudioStream newst(outparams, inparams, format, sampleRate, bufferSize, callback, args, options);
    activeStreams.insert(&newst);
    return newst;
};
void AudioBackend::closeStream(AudioStream* stream) {
    activeStreams.erase(stream);
}
*/

// wanted to make AudioBackend abstract. :D
// now everything is in comments

Toluene::AudioBackend::AudioBackend(Api api) {
    currentApi = api; // i seriously have no idea what the point of this is
}