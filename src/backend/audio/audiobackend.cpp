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

Toluene::AudioDevice::AudioDevice(AudioBackend* bcknd) {
    owner = bcknd;
};

Toluene::AudioBackend::AudioBackend(Api api) {
    currentApi = api; // i seriously have no idea what the point of this is
}