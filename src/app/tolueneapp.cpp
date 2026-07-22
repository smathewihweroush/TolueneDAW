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

#include "tolueneapp.h"

#include <audiobackend.h>
#include <cstdlib>
#include <ncursestui.h>
#include <rtaudiobackend.h>
#include <memory>

void TolueneApp::start() {
    audioBackend = std::make_shared<RtAudioBackend>(Toluene::LINUX_PULSE);
    tui = std::make_shared<NcursesTui>();
    tui->begin(true);

    setConfigs();
}

void TolueneApp::setConfigs() {
    audioConfig.addData("defaultAudioOutDevice", "string", true);
    audioConfig.addData("defaultAudioInDevice", "string", true);
}

int TolueneApp::loop() {
    tui->winaddstr(tui->mainwin, L"F°ne ↓nd dændy. W¶rks |n h”pes a¢d drea§s.\n");
    tui->wingetchar(tui->mainwin);
    return EXIT_SUCCESS;
}

TolueneApp::TolueneApp() {

}

TolueneApp::~TolueneApp() {

}