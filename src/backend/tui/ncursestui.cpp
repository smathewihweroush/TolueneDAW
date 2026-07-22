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
#include <bits/types/wint_t.h>
#include <curses.h>
#include <iostream>
#include <ncurses.h>
#include <ncursestui.h>
#include <string>
#include <sys/types.h>
#include <utility>

bool NcursesTui::hasNotStarted() {
    if (!started) {
        std::cerr << "Did not start Tui.\n";
        return true;
    }
    return false;
}

void NcursesTui::begin(bool needsColor) {
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

void NcursesTui::stop() {
    windowPairs.clear();
    endwin();
    started = 0;
} // TODO: stub

void NcursesTui::addchar(wchar_t character) {
    if (hasNotStarted()) return;
    winaddchar(mainwin, character);
}

void NcursesTui::addstring(std::wstring string) {
    if (hasNotStarted()) return;
    winaddstr(mainwin, string);
}

void NcursesTui::winaddchar(Toluene::WindowId window, Toluene::wchar character) {
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

void NcursesTui::winaddstr(Toluene::WindowId window, std::wstring string) {
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
    waddwstr(win, string.c_str());
}

Toluene::InputEvent NcursesTui::getchar() {
    if (hasNotStarted()) return {}; // TODO: what here?
    return wingetchar(mainwin);
}

Toluene::InputEvent NcursesTui::wingetchar(Toluene::WindowId windowId) {
    if (hasNotStarted()) return {};
    if (windowId == 0) {
        std::cerr << "Given WindowId (0) doesn't point to anything, or there's a bug in the code.\n";
        return {};
    }
    // find the window
    WINDOW* win = nullptr;
    for (int i = 0; i < windowPairs.size(); i++) {
        if (windowPairs[i].first == windowId) {
            win = windowPairs[i].second;
            break;
        }
    }
    if (win == nullptr) {
        std::cerr << "Could not find requested window.\n";
        return {};
    }
    unsigned int ch;
    int st = wget_wch(win, &ch);
    Toluene::InputEvent inp {ch, {}, 0, 0}; // TODO: no conversion between keycode
    if (st == KEY_CODE_YES) {
        inp.isKeycode = 1;
        if (st == KEY_MOUSE) {
            inp.isMouse = 1;
            Toluene::MouseInfo msf;
            MEVENT mev;
            getmouse(&mev);
            msf.events = mev.bstate; // TODO: no conversion between bstatemkwfkwkgmjnitrejb w
            msf.x = mev.x;
            msf.y = mev.y;
            inp.mouseInfo = msf;
        }
    } else if (st == ERR) {
        std::cerr << "Ncurses encountered into an error.\n"; // TODO: what to do?
        return {};
    }
    return inp;
}

void NcursesTui::drawall() {
    // TODO: stub
}

NcursesTui::NcursesTui() {
    started = false;
    mainwin = 0;
}

NcursesTui::~NcursesTui() {
    if (started == true) {
        stop();
    }
}