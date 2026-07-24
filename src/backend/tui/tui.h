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
    // a number representing key presses. most of the time it is the character itself.
    // other times it represents a keycode corresponding to most of the keycodes in ncurses
    typedef long int Keycode;
    typedef unsigned long long int MouseMask; // a bitmask with bits representing whether certain mouse events were triggered
    typedef wchar_t wchar; // wide character for unicode

    
    class Tui;
    class Window { // contains information for a window
        public:
        virtual void draw() {}; // function that gets called before this window gets drawn (updated) onto the screen
        
        WindowId id; // the toluene id of a window instance
        int index = 0; // the "z position" of the window, lower index windows get drawn first, can be negative
        //bool visible; // is the window visible? // this could be cool, but for my own sanity, let's not do this for now
        Tui* tui; // tui instance which controls this window
        int x = 0; // the x component of the position of the window on the console, in characters
        int y = 0; // the y component of the position of the window on the console, in characters
        int height; // the height of the window, in characters
        int width; // the width of the window, in characters

        // create a new window with width and height, at default (0, 0) position
        Window(int height, int width) : height(height), width(width) {};
        // create a new window at a position with width and height specified
        Window(int x, int y, int height, int width) : x(x), y(y), height(height), width(width) {};
        // create a new window at a position with dimensions and z index specified
        Window(int x, int y, int height, int width, int index) : x(x), y(y), height(height), width(width), index(index) {};
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
        virtual void begin(bool needsColor) = 0; // set up screen, main window, and any backends
        virtual void stop() = 0;
        // basic writing
        virtual void addchar(wchar character) = 0; // adds a single character to the main window
        virtual void addstring(std::wstring string) = 0; // adds a string to the main window
        virtual void winaddchar(WindowId windowId, wchar character) = 0; // adds a single character to a window
        virtual void winaddstr(WindowId windowId, std::wstring) = 0; // adds a string to a window
        // basic reading
        virtual InputEvent getchar() = 0; // despite the name, this function returns an input event handled at the mainwin
        virtual InputEvent wingetchar(WindowId windowId) = 0; // like getchar(), but for any given window
        // drawing
        virtual void drawall() = 0;
        // window manipulation
        virtual WindowId addwin(Window* holder) = 0; // adds a window

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