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
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <rtaudiobackend.h>
#include <ncurses.h>
#include <unistd.h>

using namespace std;
using namespace Toluene;

int some = 512;
int sound(void *out, void *in, unsigned int nFrames, double time, AudioStreamStatus stat, void* usrd) {
    unsigned int i, j;
    // we dont do any status checking, but thats surely aaaaaalright!!!
    int16_t *buf = (int16_t*) out;
    for (i = 0; i < nFrames; i++) {
        for (j = 0; j < 2; j++) {
            *buf = (i % some) * 20;
            ++buf;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    // choose api
    vector<Toluene::Api> apsi = RtAudioBackend::getAvailableApis();
    cout << "Choose the id of which Api to use for audio playback. Options are:\n";
    for (int i = 0; i < apsi.size(); i++) {
        cout << "Id " << apsi[i] << ": " << RtAudioBackend::getApiName(apsi[i]) << '\n';
    }
    Toluene::Api chosenApi;
    cin >> (int&)chosenApi;
    cout << '\n';
    // set up
    unique_ptr<AudioBackend> bcknd = make_unique<RtAudioBackend>(chosenApi);
    bcknd->startApi();
    cout << "Created Backend.\n";
    cout << "Let's try to make audio output! I'll try to use your default audio output device. \n";
    AudioStreamParameters out{bcknd->getDefaultOutputDevice()->deviceId, 2, 0};
    unsigned int bufferframes = 512;
    AudioStreamId id = bcknd->openStream(&out, NULL, {Toluene::SINT16}, 44100, &bufferframes, &sound);
    // we can hear sound...
    bcknd->startStream(id); // now!!!
    while (true) {
        // to exit this loop, just ctrl+c keyboard interrupt
        cout <<"Enter some positive integer: ";
        cin >> some;
    }
    bcknd->closeStream(id); // automatically also stops the stream!
    
    return EXIT_SUCCESS;
}
