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

#include <memory>
#include <string>
#include <vector>

namespace Toluene {
    typedef unsigned int WindowId; // an id for a window, 0 means nothing, otherwise may be an index, but no guarantees
    typedef long int Keycode; // a number representing key presses. most of the time it is the character itself
    typedef unsigned long long int MouseMask; // a bitmask with bits representing whether certain mouse events were triggered

    
    class Tui;
    class Window { // contains information for a window
        public:
        void draw() {}; // function that gets called each time tui needs to draw this window
        
        WindowId id; // the id of a window instance
        int index; // the position of the window, lower index windows get drawn first (therefore being in the background)
        bool visible; // is the window visible?
        Tui* tui; // tui instance which controls this window 
    };

    struct MouseInfo {
        int x, y; // the x and y position of the 
        MouseMask events;
    };
    struct InputEvent {
        Keycode keycode; // the keycode of the input event, for button presses that arent characters
        MouseInfo mouseInfo; // in the case of a mouse event, this struct contains all mouse event information
        bool isKeycode; // is this a special input event unlike a regular character?
        bool isMouse; // is this a mouse event?
    };

    class Tui {
        public:
        // initialization
        virtual void initialize(bool needsColor) = 0; // set up screen, main window, and any backends
        // basic writing
        virtual void addchar(wchar_t character) = 0; // adds a single character to the main window
        //virtual void addstr(std::wstring string) = 0; // adds a string to the main window
        virtual void winaddchar(WindowId windowId, wchar_t character) = 0; // adds a single character to a window
        //virtual void winaddstr(WindowId windowId, std::wstring) = 0; // adds a string to a window
        // drawing
        virtual void drawall() = 0;
        // input

        // tui
        Tui() {};
        virtual ~Tui() {};

        bool started; // is the tui active?
        WindowId mainwin; // the id of the main window which is always existent and 1 if tui is active, 0 otherwise
        protected:
        std::vector<std::shared_ptr<Window>> windows;
    };
}

// main()create new win