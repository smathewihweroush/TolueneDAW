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

#include "tui.h"
#include <curses.h>
#include <iostream>
#include <ncurses.h>
#include <ncursestui.h>
#include <utility>

bool NcursesTui::hasNotStarted() {
    if (!started) {
        std::cerr << "Did not start Tui.\n";
        return true;
    }
    return false;
}

void NcursesTui::initialize(bool needsColor) {
    if (started) {
        std::cerr << "Already started Tui.\n";
        return;
    }
    initscr();
    bool colors = has_colors();
    if (needsColor && (colors == FALSE)) {
        std::cerr << "This terminal does not support color.\n";
        endwin();
        return;
    }
    if (colors == TRUE) {
        start_color();
    }
    windowPairs.push_back(std::make_pair(1, stdscr));
    mainwin = 1;
    started = true;
}

void NcursesTui::addchar(wchar_t character) {
    if (hasNotStarted()) return;
    winaddchar(mainwin, character);
}

void NcursesTui::winaddchar(Toluene::WindowId window, wchar_t character) {
    if (hasNotStarted()) return;
    if (window == 0) {
        std::cerr << "Given WindowId (0) doesn't point to anything, or there's a bug in the code.\n";
        return;
    }
    // find the window
    WINDOW* win = nullptr;
    for (int i = 0; i < windowPairs.size(); i++) {
        if (windowPairs[i].first == window) {
            win = windowPairs[i].second;
            break;
        }
    }
    if (win == nullptr) {
        std::cerr << "Could not find requested window.\n";
        return;
    }
    cchar_t ch;
    setcchar(&ch, &character, 0, 0, nullptr);
    wadd_wch(win, &ch);
}

void NcursesTui::drawall() {

}

NcursesTui::NcursesTui() {
    started = false;
    mainwin = 0;
}

NcursesTui::~NcursesTui() {
    if (started == true) {
        // TODO: stub, should stopwin and stuff
    }
}