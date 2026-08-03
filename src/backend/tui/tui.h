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
    // an id for a window, 0 means nothing, otherwise may be an index, but no guarantees
    typedef unsigned int WindowId;
    /*  a number representing key presses. most of the time it is the character itself.
        other times it represents a keycode corresponding to most of the keycodes in ncurses */
    typedef long int Keycode;
    // a bitmask with bits representing whether certain mouse events were triggered
    typedef unsigned long long int MouseMask;
    // wide character for unicode
    typedef wchar_t wchar;

    // the available ways the tui can configure user's console to handle input from the user.
    enum ConsoleInputMode {
        // at the beginning the console configuration is ambiguous, so UNSET signifies that ambiguity.
        UNSET,
        // cooked mode buffers input until enter key is pressed
        COOKED,
        // unlike cooked, does not buffer, but interprets escape sequences, and allows control of 
        // input flow with special characters. also signals something something TODO
        CBREAK,
        // like cbreak, but furthermore passes characters from escape sequences. signals are ignored.
        RAW
    };

    // the available ways the tui can configure user's console echoing
    enum EchoMode {
        // input is echoed to mainwin
        ECHO,
        // input is not echoed anywhere
        NOECHO
    };
    
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
        // the x position of the window on the console, in characters
        int x = 0;
        // the y position of the window on the console, in characters
        int y = 0;
        // the height of the window, in characters
        int height;
        // the width of the window, in characters
        int width;

        // create a new window with width and height, at default (0, 0) position
        Window(int height, int width) : height(height), width(width) {};
        // create a new window at a position with width and height specified, as well as an optional index
        Window(int x, int y, int height, int width, int index = 0) : x(x), y(y), height(height), width(width), index(index) {};
    };

    // the class of the mainwin
    class MainWindow : public Window {
        public:
        // empty because it's equivalent to a regular window for now
        MainWindow() : Window(0, 0) {}; // just need a constructor
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

    // R.I.P. WindowSafe class, i had doubts in you anyaways so idc

    // an entirely opaque, implementation-defined abstract class which serves as the interface for making tui.
    // contains merely the required utilities, derived classes may have more functionality, but what they add shall
    // not be used publicly (except for debugging purposes) if implementations are to be interchangable.
    class Tui {
            public: //          ---- public stuff ----
        
        // control

        // set up screen, main window, and any backends
        virtual void begin(bool needsColor) = 0;
        // delete windows, and stop underlying backends
        virtual void stop() = 0;

        // input behavior control

        // try to set console input mode
        virtual void setMode(ConsoleInputMode mode) = 0;
        // try to get current console input mode
        virtual ConsoleInputMode getMode() = 0;
        // try to set echo mode
        virtual void setEcho(EchoMode mode) = 0;
        // try to get curretn echo mode
        virtual EchoMode getEcho() = 0;

        // basic writing

        // adds a single character to the main window
        virtual void addChar(wchar character) = 0;
        // adds a string to the main window
        virtual void addString(std::wstring string) = 0;
        // adds a single character to a window
        virtual void winAddChar(WindowId windowId, wchar character) = 0;
        // adds a string to a window
        virtual void winAddStr(WindowId windowId, std::wstring) = 0;

        // adds a horizontal line of character ch with specified length n to the mainwin
        virtual void hLine(wchar ch, int n) = 0;
        // adds a vertical line of character ch with specified length n to the mainwin
        virtual void vLine(wchar ch, int n) = 0;
        // adds a horizontal line of character ch with specified length n to a window
        virtual void winHLine(WindowId windowId, wchar ch, int n) = 0;
        // adds a vertical line of character ch with specified length n to a window
        virtual void winVLine(WindowId windowId, wchar ch, int n) = 0;
        // fills a rectangular area with a single character on the mainwin
        virtual void fill(wchar ch, int w, int h) = 0;
        // fills a rectangular area with a single character on a window
        virtual void winFill(WindowId windowId, wchar ch, int w, int h) = 0;
        
        // sets characters to use for the bordered box in box() and winBox()
        // if a character is null, it will not draw anything
        // tl = top-left, tm = top-middle, tr = top-right
        // cl = center-left, cm = center-middle, cr = center-right
        // bl = bottom-left, bm = bottom-middle, br = bottom-right
        virtual void setBox(wchar tl, wchar tm, wchar tr, wchar cl, wchar cm, wchar cr, wchar bl, wchar bm, wchar br) = 0;
        // adds a bordered box to the main window
        // (x1, y1) is the position of the top-left corner, (x2, y2) of the bottom-right corner
        virtual void box(int x1, int y1, int x2, int y2) = 0;
        // adds a bordered box to a window, uses border characters set by setBrd
        // (x1, y1) is the position of the top-left corner, (x2, y2) of the bottom-right corner
        virtual void winBox(WindowId windowId, int x1, int y1, int x2, int y2) = 0;

        // basic reading

        // despite the name, this function returns an input event handled at the mainwin
        virtual InputEvent getChar() = 0; 
        // like getchar(), but for any given window
        virtual InputEvent winGetChar(WindowId windowId) = 0;

        // basic drawing

        // this is the main function recommended for drawing. draws all windows in the correct order.
        virtual void drawAll() = 0;
        // this draws a single window. not in order.
        virtual void drawWin(WindowId windowId) = 0;

        // window manipulation

        // add a dynamically allocated window for the tui to handle.
        virtual WindowId addWin(std::shared_ptr<Window> holder) = 0;
        // safely delete a dynamically allocated window.
        virtual void delWin(WindowId windowId) = 0;
        // safely delete all windows, not including the main window which is always existent.
        virtual void delAllW() = 0;
        // move mainwin's cursor.
        virtual void mv(int x, int y) = 0;
        // move a window's cursor
        virtual void winMv(WindowId windowId, int x, int y) = 0; 

        // utility

        // get toluene window pointer from id. if not found, return nullptr
        virtual Window* getTolWin(Toluene::WindowId windowId) = 0;

        // tui

        // constructor of the tui, excepts to default everything, but not start anything yet.
        Tui() {};
        // destructor of the tui, excepts to release everything (free handled memory), and stop any backends.
        virtual ~Tui() {};

        // public variables

        // is the tui active?
        bool started;
        // the id of the main window which should always be existent, is specially handled, and is 0 if tui is inactive
        WindowId mainWin;
        // all the various characters used by the bordered box in certain functions. check setBrd().
        wchar tl = '\0', tm = '\0', tr = '\0', cl = '\0', cm = '\0', cr = '\0', bl = '\0', bm = '\0', br = '\0';

            protected: //       ---- private stuff ----

        // control

        // safely release everything this tui instance controls, like windows, and anything the backend handles. call in stop().
        virtual void release() = 0;

        // window manipulation

        // either add or update mainwin to match the main window (if one exists) in any backend from an implementation
        virtual void queryMainWin() = 0;

        // protected variables

        // vector containing pointers to dynamically allocated windows.
        std::vector<std::shared_ptr<Window>> windows;
        // tui's current console input mode
        ConsoleInputMode inputMode;
        // tui's current echo mode
        EchoMode echoMode = ECHO;
    };
}

// main()create new win