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

#include <ncurses.h>
#include <string>
#include <tui.h>
#include <vector>

class NcursesTui : public Toluene::Tui {
    public:
    // starting and stopping
    void begin(bool) override;
    void stop() override;

    // basic writing
    void addchar(Toluene::wchar character) override;
    void addstring(std::wstring string) override;
    void winaddchar(Toluene::WindowId windowId, Toluene::wchar character) override;
    void winaddstr(Toluene::WindowId windowId, std::wstring character) override;

    // basic reading
    Toluene::InputEvent getchar() override; 
    Toluene::InputEvent wingetchar(Toluene::WindowId windowId) override;

    // drawing
    void drawall() override;
    void drawwin(Toluene::WindowId windowId) override;

    // window manipulation
    Toluene::WindowId addwin(Toluene::Window* holder) override;
    void delwin(Toluene::WindowId windowId) override;
    void delallw() override;

    // ncursestui
    NcursesTui();
    ~NcursesTui() override;

    // utility

    // get ncurses window pointer from id. if not found, return nullptr
    WINDOW* getncwin(Toluene::WindowId windowId);

    private:
    // control

    void release() override;

    // utility

    // utility which warns the user if they haven't initialized the tui. returns true if hasn't started
    bool hasNotStarted();

    // private variables

    // windowids correspond to ncurses pointers in this implementation
    // and to track these pairs, we just use a vector of pairs
    std::vector<std::pair<Toluene::WindowId, WINDOW*>> windowPairs;
    // "bitmask" array used to mark which window ids are used. this implies a limit of 998 windows.
    // window ids 0 and 1 are reserved.
    bool used[1000];
};