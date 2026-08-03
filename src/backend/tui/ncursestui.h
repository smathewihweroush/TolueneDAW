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
    // control
    void begin(bool) override;
    void stop() override;

    // input behavior control

    void setMode(Toluene::ConsoleInputMode mode) override;
    Toluene::ConsoleInputMode getMode() override;
    void setEcho(Toluene::EchoMode mode) override;
    Toluene::EchoMode getEcho() override;

    // basic writing

    void addChar(Toluene::wchar character) override;
    void addString(std::wstring string) override;
    void winAddChar(Toluene::WindowId windowId, Toluene::wchar character) override;
    void winAddStr(Toluene::WindowId windowId, std::wstring character) override;

    void hLine(Toluene::wchar ch, int n) override;
    void vLine(Toluene::wchar ch, int n) override;
    void winHLine(Toluene::WindowId windowId, Toluene::wchar ch, int n) override;
    void winVLine(Toluene::WindowId windowId, Toluene::wchar ch, int n) override;
    void fill(Toluene::wchar ch, int w, int h) override;
    void winFill(Toluene::WindowId windowId, Toluene::wchar ch, int w, int h) override;
    
    void setBox(Toluene::wchar tl, Toluene::wchar tm, Toluene::wchar tr, Toluene::wchar cl, 
        Toluene::wchar cm, Toluene::wchar cr, Toluene::wchar bl, Toluene::wchar bm, Toluene::wchar br) override;
    void box(int x1, int y1, int x2, int y2) override;
    void winBox(Toluene::WindowId windowId, int x1, int y1, int x2, int y2) override;

    // basic reading

    Toluene::InputEvent getChar() override; 
    Toluene::InputEvent winGetChar(Toluene::WindowId windowId) override;

    // drawing

    void drawAll() override;
    void drawWin(Toluene::WindowId windowId) override;

    // window manipulation

    Toluene::WindowId addWin(std::shared_ptr<Toluene::Window> holder) override;
    void delWin(Toluene::WindowId windowId) override;
    void delAllW() override;
    void mv(int x, int y) override;
    void winMv(Toluene::WindowId windowId, int x, int y) override; 

    // utility

    Toluene::Window* getTolWin(Toluene::WindowId windowId) override;
    // get ncurses window pointer from id. if not found, return nullptr
    WINDOW* getNcWin(Toluene::WindowId windowId);

    // ncursestui

    NcursesTui();
    ~NcursesTui() override;

    private:
    // control

    void release() override;

    // window manipulation

    void queryMainWin() override;

    // utility

    // utility which warns the user if they haven't initialized the tui. returns true if hasn't started
    bool hasNotStarted();
    // returns a plain cchar_t with character ch 
    cchar_t chToCC(Toluene::wchar ch);

    // private variables

    // windowids correspond to ncurses pointers in this implementation
    // and to track these pairs, we just use a vector of pairs
    std::vector<std::pair<Toluene::WindowId, WINDOW*>> windowPairs;
    // "bitmask" array used to mark which window ids are used. this implies a limit of 998 windows.
    // window ids 0 and 1 are reserved
    bool used[1000];
};