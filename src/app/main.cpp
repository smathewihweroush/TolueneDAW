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
#include <memory>
#include <rtaudiobackend.h>
#include <ncurses.h>
#include <string>
#include <unistd.h>

using namespace std;
using namespace Toluene;

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
    
    cout <<"Enter something to end: ";
    string nthn;
    cin >> nthn;
    
    return 0;
}
