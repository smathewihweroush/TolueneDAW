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
#include <algorithm>
#include <bits/types/wint_t.h>
#include <curses.h>
#include <iostream>
#include <ncurses.h>
#include <ncursestui.h>
#include <string>
#include <sys/types.h>
#include <utility>
#include <vector>

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
    if (hasNotStarted()) return;
    release();
    started = 0;
    mainwin = 0;
} // TODO: stub, possibly

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
    WINDOW* win = getncwin(window);
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
    WINDOW* win = getncwin(window);
    if (win == nullptr) {
        std::cerr << "Could not find requested window.\n";
        return;
    }
    waddwstr(win, string.c_str());
}

Toluene::InputEvent NcursesTui::getchar() {
    if (hasNotStarted()) return {0};
    return wingetchar(mainwin);
}

Toluene::InputEvent NcursesTui::wingetchar(Toluene::WindowId windowId) {
    if (hasNotStarted()) return {0};
    if (windowId == 0) {
        std::cerr << "Given WindowId (0) doesn't point to anything, or there's a bug in the code.\n";
        return {0};
    }
    WINDOW* win = getncwin(windowId);
    if (win == nullptr) {
        std::cerr << "Could not find requested window.\n";
        return {0};
    }
    unsigned int ch;
    int st = wget_wch(win, &ch);
    Toluene::InputEvent inp {ch, {}, 0, 0, 1}; // TODO: no conversion between keycode
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
        return {0};
        // TODO: work through errors which are actually problematic
    }
    return inp;
}

void NcursesTui::drawall() {
    if (hasNotStarted()) return;
    std::vector<std::pair<int, Toluene::WindowId>> wins;
    wins.push_back(std::make_pair(0, mainwin)); // we casually assume the standard screen is at index 0
    for (int i = 0; i < windows.size(); i++) {
        wins.push_back(std::make_pair(windows[i]->index, windows[i]->id));
    }
    std::sort(wins.begin(), wins.end());
    for (int i = 0; i < wins.size(); i++) {
        drawwin(wins[i].second);
    }
}

void NcursesTui::drawwin(Toluene::WindowId windowId) {
    if (hasNotStarted()) return;
    if (windowId == 0) {
        std::cerr << "Given window id for drawing is not valid.\n";
        return;
    }
    if (windowId != 1) {
        bool drawn = 0;
        for (int i = 0; i < windows.size(); i++) {
            if (windows[i]->id == windowId) {
                windows[i]->draw();
                drawn = 1;
                break;
            }
        }
        if (drawn == 0) {
            std::cerr << "Could not find Toluene window from id.\n";
            return;
        }
    }
    WINDOW* hj = getncwin(windowId);
    wrefresh(hj);
}

Toluene::WindowId NcursesTui::addwin(Toluene::Window* holder) {
    if (hasNotStarted()) return 0;
    WINDOW* win = newwin(holder->height, holder->width, holder->y, holder->x);
    if (win == nullptr) {
        std::cerr << "Error: Ncurses returned nullptr for new window.\n";
        return 0;
    }
    Toluene::WindowId newwinid = 0;
    for (int i = 0; i < 1000; i++) {
        if (used[i] == 0) {
            newwinid = i;
            break;
        }
    }
    if (newwinid == 0) {
        std::cerr << "Could not find an available window id for new window.\n";
        return 0;
    }
    holder->id = newwinid;
    holder->tui = this;
    windows.emplace_back(holder);
    windowPairs.emplace_back(newwinid, win);
    return newwinid;
}

void NcursesTui::delwin(Toluene::WindowId windowId) {
    if (hasNotStarted()) return;
    int windex = -1;
    for (int i = 0; i < windows.size(); i++) {
        if (windows[i]->id == windowId) {
            windex = i;
            break;
        }
    }
    int pindex = -1;
    for (int i = 0; i < windowPairs.size(); i++) {
        if (windowPairs[i].first == windowId) {
            pindex = i;
            break;
        }
    }
    if (windex == -1) {
        std::cerr << "Could not find Toluene window from window id.\n";
        return;
    }
    if (pindex == -1) {
        std::cerr << "Could not find window pair with ncurses window from window id.\n";
        return;
    }
    ::delwin(windowPairs[pindex].second);
    windowPairs.erase(std::next(windowPairs.begin(), pindex));
    windows.erase(std::next(windows.begin(), windex));
}

void NcursesTui::delallw() {
    if (hasNotStarted()) return;
    int s = 0;
    for (int i = windows.size() - 1; i >= 0; i++) {
        s = windows.size();
        delwin(windows[i]->id);
        if (s != (windows.size() - 1)) { // basic error check, just so i can see if it works right
            std::cerr << "Window count doesn't match what it should be after deleting a single window.\n";
            std::cerr << "Previous count: " << s << ", current count: " << windows.size() << '\n';
            return;
        }
    }
}

NcursesTui::NcursesTui() {
    started = false;
    mainwin = 0;
    used[0] = 1;
    used[1] = 1;
}

NcursesTui::~NcursesTui() {
    if (started == true) {
        stop();
    }
}

WINDOW* NcursesTui::getncwin(Toluene::WindowId windowId) {
    if (hasNotStarted()) return nullptr;
    WINDOW* win = nullptr;
    for (int i = 0; i < windowPairs.size(); i++) {
        if (windowPairs[i].first == windowId) {
            win = windowPairs[i].second;
            break;
        }
    }
    return win;
}

void NcursesTui::release() {
    delallw();
    // we still have mainwin!
    if (!windowPairs.empty()) {
        windowPairs.clear(); // we should be able to do this safely
    } else {
        std::cerr << "Warning: window pairs are empty in release function after deleting all dynamic windows. No main windows is left.\n";
        // TODO: what to do?
    }
    endwin();
}

bool NcursesTui::hasNotStarted() {
    if (!started) {
        std::cerr << "Did not start Tui.\n";
        return true;
    }
    return false;
}