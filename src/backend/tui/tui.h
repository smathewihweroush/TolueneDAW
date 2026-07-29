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

#include <curses.h>
#include <string>
#include <vector>

namespace Toluene {
    // an id for a window, 0 means nothing, otherwise may be an index, but no guarantees
    typedef unsigned int WindowId;
    /*  a number representing key presses. most of the time it is the character itself.
        other times it represents a keycode corresponding to most of the keycodes in ncurses */
    typedef long int Keycode;
    // a bitmask with bits representing whether certain mouse events were triggered
    typedef unsigned long long int MouseMask;
    // wide character for unicode
    typedef wchar_t wchar;
    
    class Tui;
    // a window class contains all information for rendering a window, as well as some useful functions
    class Window {
        public:
        // function that gets called before this window gets drawn (updated) onto the screen
        virtual void draw() {};
        
        // the toluene id of a window instance
        WindowId id;
        // the "z position" of the window, lower index windows get drawn first, can be negative
        int index = 0;
        //bool visible; // is the window visible? // this could be cool, but for my own sanity, let's not do this for now
        // tui instance which controls this window
        Tui* tui;
        // the x component of the position of the window on the console, in characters
        int x = 0;
        // the y component of the position of the window on the console, in characters
        int y = 0;
        // the height of the window, in characters
        int height;
        // the width of the window, in characters
        int width;

        // create a new window with width and height, at default (0, 0) position
        Window(int height, int width) : height(height), width(width) {};
        // create a new window at a position with width and height specified
        Window(int x, int y, int height, int width) : x(x), y(y), height(height), width(width) {};
        // create a new window at a position with dimensions and z index specified
        Window(int x, int y, int height, int width, int index) : x(x), y(y), height(height), width(width), index(index) {};
    };

    // struct for info provided on a mouse event
    struct MouseInfo {
        // the x and y position of the mouse event
        int x, y;
        // all mouse events that happened (buttons which were pressed, corresponds to ncurses)
        MouseMask events;
    };
    // information which is provided when any input is made
    struct InputEvent {
        Keycode keycode; // the keycode of the input event, for button presses that arent characters
        MouseInfo mouseInfo; // in the case of a mouse event, this struct contains all mouse event information
        bool isKeycode; // is this a special input event unlike a regular character?
        bool isMouse; // is this a mouse event?
        bool valid = 0; // does this inputevent even contain valid data?
        InputEvent();
        InputEvent(bool validity) : valid(validity) {}
        InputEvent(Keycode kc, MouseInfo mi, bool ik, bool im, bool v)
            : keycode(kc), mouseInfo(mi), isKeycode(ik), isMouse(im), valid(v) {}
    };

    // a class which contains a pointer to a dynamically allocated window, and deletes it when destructed.
    // please don't delete the pointer manually.  
    class WindowSafe {
        public:
        // contained pointer to dynamically allocated window
        Window* p;
        // utility operator to allow similar access like a pointer
        Window* operator->();
        // constructor
        WindowSafe(Window* ptr) : p(ptr) {}
        // destructor, should also delete pointer
        ~WindowSafe();
    };

    // an entirely opaque, implementation-defined abstract class which serves as the interface for making tui.
    // contains merely the required utilities, derived classes may have more functionality, but what they add shall
    // not be used publicly (except for debugging purposes) if implementations are to be interchangable.
    class Tui {
        public:
        // control

        // set up screen, main window, and any backends
        virtual void begin(bool needsColor) = 0;
        // delete windows, and stop underlying backends
        virtual void stop() = 0;

        // basic writing

        // adds a single character to the main window
        virtual void addchar(wchar character) = 0;
        // adds a string to the main window
        virtual void addstring(std::wstring string) = 0;
        // adds a single character to a window
        virtual void winaddchar(WindowId windowId, wchar character) = 0;
        // adds a string to a window
        virtual void winaddstr(WindowId windowId, std::wstring) = 0;

        // basic reading

        // despite the name, this function returns an input event handled at the mainwin
        virtual InputEvent getchar() = 0; 
        // like getchar(), but for any given window
        virtual InputEvent wingetchar(WindowId windowId) = 0;

        // basic drawing

        // this is the main function recommended for drawing. draws all windows in the correct order.
        virtual void drawall() = 0;
        virtual void drawwin(WindowId windowId) = 0;
        // window manipulation
        // add a dynamically allocated window for the tui to handle.
        virtual WindowId addwin(Window* holder) = 0;
        // safely delete a dynamically allocated window.
        virtual void delwin(WindowId windowId) = 0;
        // safely delete all windows, not including the main window which is always existent.
        virtual void delallw() = 0;

        // tui

        Tui() {};
        virtual ~Tui() {};

        // public variables

        // is the tui active?
        bool started;
        // the id of the main window which is always existent and equal to 1 if tui is active, 0 otherwise
        WindowId mainwin;

        protected:
        // control

        // safely release everything this tui instance controls, like windows, and anything the backend handles. call in stop().
        virtual void release() = 0;

        // protected variables

        // vector containing pointers to dynamically allocated windows.
        std::vector<WindowSafe> windows;
    };
}

// main()create new win