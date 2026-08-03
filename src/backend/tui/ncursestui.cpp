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
#include <cstddef>
#include <curses.h>
#include <iostream>
#include <ncurses.h>
#include <ncursestui.h>
#include <string>
#include <sys/types.h>
#include <utility>
#include <vector>

using namespace Toluene;

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
    started = true;
    //windowPairs.push_back(std::make_pair(1, stdscr));
    //mainwin = 1;
    queryMainWin();
    setMode(RAW);
}

void NcursesTui::stop() {
    if (hasNotStarted()) return;
    release();
    started = 0;
    mainWin = 0;
    inputMode = UNSET;
} // TODO: stub, possibly

void NcursesTui::setMode(ConsoleInputMode cim) {
    if (hasNotStarted()) return;
    // these unsetting-setting shenanigans may be unnecessary but i can't be bothered to risk it
    if (inputMode == cim) return;
    if (inputMode == UNSET) {
        cbreak(); // although setting to cbreak is arbitrary, it does guarantee the console input mode is cbreak!...
        inputMode = CBREAK;
    }
    switch (cim) {
        case COOKED:
            if (inputMode == RAW) noraw(); 
            else nocbreak();
            break;
        case CBREAK:
            if (inputMode == RAW) noraw(); 
            cbreak();
            break;
        case RAW:
            if (inputMode == CBREAK) nocbreak();
            raw();
            break;
        default:
            std::cerr << "Provided ConsoleInputMode is invalid, or should not be used.\n";
            return;
    }
    inputMode = cim;
}

ConsoleInputMode NcursesTui::getMode() {
    hasNotStarted();
    return inputMode; // easy!
}

void NcursesTui::setEcho(EchoMode mode) {
    if (hasNotStarted()) return;
    if (mode == echoMode) return;
    switch (echoMode) {
        case ECHO:
            echo();
            break;
        case NOECHO:
            noecho();
            break;
    }
    echoMode = mode;
}

EchoMode NcursesTui::getEcho() {
    hasNotStarted();
    return echoMode;
}

void NcursesTui::addChar(wchar_t character) {
    if (hasNotStarted()) return;
    winAddChar(mainWin, character);
}

void NcursesTui::addString(std::wstring string) {
    if (hasNotStarted()) return;
    winAddStr(mainWin, string);
}

void NcursesTui::winAddChar(WindowId window, wchar character) {
    if (hasNotStarted()) return;
    if (window == 0) {
        std::cerr << "Given WindowId (0) doesn't point to anything, or there's a bug in the code.\n";
        return;
    }
    WINDOW* win = getNcWin(window);
    if (win == nullptr) {
        std::cerr << "Could not find requested window.\n";
        return;
    }
    cchar_t ch;
    setcchar(&ch, &character, 0, 0, nullptr);
    wadd_wch(win, &ch);
}

void NcursesTui::winAddStr(WindowId window, std::wstring string) {
    if (hasNotStarted()) return;
    if (window == 0) {
        std::cerr << "Given WindowId (0) doesn't point to anything, or there's a bug in the code.\n";
        return;
    }
    WINDOW* win = getNcWin(window);
    if (win == nullptr) {
        std::cerr << "Could not find requested window.\n";
        return;
    }
    waddwstr(win, string.c_str());
}

void NcursesTui::hLine(wchar ch, int n) {
    if (hasNotStarted()) return;
    winHLine(mainWin, ch, n);
}

void NcursesTui::vLine(wchar ch, int n) {
    if (hasNotStarted()) return;
    winVLine(mainWin, ch, n);
}

void NcursesTui::winHLine(WindowId windowId, wchar ch, int n) {
    if (hasNotStarted()) return;
    if (windowId == 0) {
        std::cerr << "Given WindowId (0) doesn't point to anything, or there's a bug in the code.\n";
        return;
    }
    WINDOW* ncwin = getNcWin(windowId);
    cchar_t cc = chToCC(ch);
    whline_set(ncwin, &cc, n);
}

void NcursesTui::winVLine(WindowId windowId, wchar ch, int n) {
    if (hasNotStarted()) return;
    if (windowId == 0) {
        std::cerr << "Given WindowId (0) doesn't point to anything, or there's a bug in the code.\n";
        return;
    }
    WINDOW* ncwin = getNcWin(windowId);
    cchar_t cc = chToCC(ch);
    wvline_set(ncwin, &cc, n);
}

void NcursesTui::fill(wchar ch, int w, int h) {
    if (hasNotStarted()) return;
    winFill(mainWin, ch, w, h);
}

void NcursesTui::winFill(WindowId windowId, wchar ch, int w, int h) {
    if (hasNotStarted()) return;
    if (windowId == 0) {
        std::cerr << "Given WindowId (0) doesn't point to anything, or there's a bug in the code.\n";
        return;
    }
    WINDOW* ncwin = getNcWin(windowId);
    int x = getcurx(ncwin), y = getcury(ncwin); // TODO: use toluene functions to get this stuff
    int by = y;
    cchar_t cc = chToCC(ch);
    for (; (y < getmaxy(ncwin)) && ((y - by) < h); y++) { // TODO: and here
        ::wmove(ncwin, y, x); // TODO: and here
        whline_set(ncwin, &cc, w);
    }
}

void NcursesTui::setBox(wchar _tl, wchar _tm, wchar _tr, wchar _cl, wchar _cm, wchar _cr, wchar _bl, wchar _bm, wchar _br) {
    tl = _tl; tm = _tm; tr = _tr;
    cl = _cl; cm = _cm; cr = _cr;
    bl = _bl; bm = _bm; br = _br;
}

void NcursesTui::box(int x1, int y1, int x2, int y2) {
    if (hasNotStarted()) return;
    winBox(mainWin, x1, y1, x2, y2);
}

void NcursesTui::winBox(WindowId windowId, int x1, int y1, int x2, int y2) {
    if (hasNotStarted()) return;
    if (windowId == 0) {
        std::cerr << "Given WindowId (0) doesn't point to anything, or there's a bug in the code.\n";
        return;
    }
    WINDOW* nc = getNcWin(windowId);
    if ((x2 >= getmaxx(nc)) || (y2 >= getmaxy(nc)) || (x1 > x2) || (y1 > y2)) { // TODO: toluene functions
        std::cerr << "Provided positions ((" << x1 << ", " << y1 << "), (" << x2 << ", " << y2 << ")) for box are invalid.\n";
    }
    int bx = getcurx(nc), by = getcury(nc); // TODO: toluene functions
    { // what is wrong with me
        if (tl != L'\0') {
            winMv(windowId, x1, y1);
            winAddChar(windowId, tl);
        }
        if (tr != L'\0') {
            winMv(windowId, x2, y1);
            winAddChar(windowId, tr);
        }
        if (bl != L'\0') {
            winMv(windowId, x1, y2);
            winAddChar(windowId, bl);
        }
        if (br != L'\0') {
            winMv(windowId, x2, y2);
            winAddChar(windowId, br);
        }
        if ((x2 - x1) > 1) {
            if (tm != L'\0') {
                winMv(windowId, x1 + 1, y1);
                winHLine(windowId, tm, (x2 - x1 - 1));
            }
            if (bm != L'\0') {
                winMv(windowId, x1 + 1, y2);
                winHLine(windowId, bm, (x2 - x1 - 1));
            }
        }
        if ((y2 - y1) > 1) {
            if (cl != L'\0') {
                winMv(windowId, x1, y1 + 1);
                winVLine(windowId, cl, (y2 - y1 - 1));
            }
            if (cr != L'\0') {
                winMv(windowId, x2, y1 + 1);
                winVLine(windowId, cr, (y2 - y1 - 1));
            }
        } 
        if (((x2 - x1) > 1) && ((y2 - y1) > 1)) {
            if (cm != L'\0') {
                winMv(windowId, x1 + 1, y1 + 1);
                winFill(windowId, cm, (x2 - x1 - 1), (y2 - y1 - 1));
            }
        }
    }
    winMv(windowId, bx, by);
}

InputEvent NcursesTui::getChar() {
    if (hasNotStarted()) return {0};
    return winGetChar(mainWin);
}

InputEvent NcursesTui::winGetChar(WindowId windowId) {
    if (hasNotStarted()) return {0};
    if (windowId == 0) {
        std::cerr << "Given WindowId (0) doesn't point to anything, or there's a bug in the code.\n";
        return {0};
    }
    WINDOW* win = getNcWin(windowId);
    if (win == nullptr) {
        std::cerr << "Could not find requested window.\n";
        return {0};
    }
    unsigned int ch;
    int st = wget_wch(win, &ch);
    InputEvent inp {ch, {}, 0, 0, 1}; // TODO: no conversion between keycode
    if (st == KEY_CODE_YES) {
        inp.isKeycode = 1;
        if (st == KEY_MOUSE) {
            inp.isMouse = 1;
            MouseInfo msf;
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

void NcursesTui::drawAll() {
    if (hasNotStarted()) return;
    std::vector<std::pair<int, WindowId>> wins;
    for (int i = 0; i < windows.size(); i++) {
        wins.push_back(std::make_pair(windows[i]->index, windows[i]->id));
    }
    std::sort(wins.begin(), wins.end());
    for (int i = 0; i < wins.size(); i++) {
        drawWin(wins[i].second);
    }
}

void NcursesTui::drawWin(WindowId windowId) {
    if (hasNotStarted()) return;
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
    WINDOW* hj = getNcWin(windowId);
    wrefresh(hj);
}

WindowId NcursesTui::addWin(std::shared_ptr<Window> holder) {
    if (hasNotStarted()) return 0;
    WINDOW* win = newwin(holder->height, holder->width, holder->y, holder->x);
    if (win == nullptr) {
        std::cerr << "Error: Ncurses returned nullptr for new window.\n";
        return 0;
    }
    WindowId newwinid = 0;
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
    windows.push_back(std::move(holder));
    windowPairs.emplace_back(newwinid, win);
    return newwinid;
}

void NcursesTui::delWin(WindowId windowId) {
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
    if (windowId != mainWin) { 
        ::delwin(windowPairs[pindex].second);
    }
    windowPairs.erase(std::next(windowPairs.begin(), pindex));
    windows.erase(std::next(windows.begin(), windex));
}

void NcursesTui::delAllW() {
    if (hasNotStarted()) return;
    int s = 0;
    for (int i = windows.size() - 1; i >= 0; i++) {
        s = windows.size();
        delWin(windows[i]->id);
        if (s != (windows.size() - 1)) { // basic error check, just so i can see if it works right
            std::cerr << "Window count doesn't match what it should be after deleting a single window.\n";
            std::cerr << "Previous count: " << s << ", current count: " << windows.size() << '\n';
            return;
        }
    }
}

void NcursesTui::mv(int x, int y) {
    if (hasNotStarted()) return;
    winMv(mainWin, x, y);
}

void NcursesTui::winMv(WindowId windowId, int x, int y) {
    if (hasNotStarted()) return;
    WINDOW* wn = getNcWin(windowId);
    ::wmove(wn, y, x);
}

Window* NcursesTui::getTolWin(WindowId windowId) {
    Window* wp = nullptr;
    for (int i = 0; i < windows.size(); i++) {
        if (windows[i]->id == windowId) {
            wp = windows[i].get();
            break;
        }
    }   
    return wp;
} 

WINDOW* NcursesTui::getNcWin(WindowId windowId) {
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

NcursesTui::NcursesTui() {
    started = false;
    mainWin = 0;
    used[0] = 1;
    used[1] = 1;
    inputMode = UNSET;
}

NcursesTui::~NcursesTui() {
    if (started == true) {
        stop();
    }
}

void NcursesTui::release() {
    delAllW();
    // we still have mainwin!
    if (!windowPairs.empty()) {
        windowPairs.clear(); // we should be able to do this safely
        std::cerr << "Warning: window pairs are not empty in release function after deleting all dynamic windows.\n";
    }
    endwin();
}

void NcursesTui::queryMainWin() {
    if (hasNotStarted()) return;
    if (mainWin != 1) {
        MainWindow* mainwininst = new MainWindow();
        WindowId id = 1;
    
        mainwininst->id = id;
        mainwininst->tui = this;

        windows.emplace_back(mainwininst);
        windowPairs.push_back(std::make_pair(id, stdscr));
        mainWin = id;
    }
    MainWindow* mainwininst = dynamic_cast<MainWindow*>(getTolWin(mainWin));
    int wid, hei;
    getmaxyx(stdscr, wid, hei); // stupid f*rea*king macro i hate how many macros ncurses has
    mainwininst->width = wid;
    mainwininst->height = hei;
}

bool NcursesTui::hasNotStarted() {
    if (!started) {
        std::cerr << "Did not start Tui.\n";
        return true;
    }
    return false;
}

cchar_t NcursesTui::chToCC(wchar ch) {
    cchar_t cch;
    setcchar(&cch, &ch, 0, 0, NULL);
    return cch;
}