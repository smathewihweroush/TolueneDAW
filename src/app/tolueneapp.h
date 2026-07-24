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

#include "config.h"
#include <tui.h>
#include <audiobackend.h>
#include <memory>

class TolueneApp {
    public:
    void start(); // initialize everything, load data, prepare for stable use
    int loop(); // main app loop, constantly waiting for input, separate thread for sound

    void setConfigs(); // initialize required config data

    TolueneApp();
    ~TolueneApp();

    std::shared_ptr<Toluene::Tui> tui; // main tui instance used by app
    //Toluene::Window win;
    std::shared_ptr<Toluene::AudioBackend> audioBackend; // main audio backend instance used by app
    
    Toluene::NamedConfig audioConfig; // configuration related to audio setup 
    
    private:

};