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
#include <tui.h>
#include <vector>

class NcursesTui : public Toluene::Tui {
    public:
    // initialization
    void initialize(bool) override;
    // basic writing
    void addchar(wchar_t character) override;
    void winaddchar(Toluene::WindowId windowId, wchar_t character) override;
    // drawing
    void drawall() override;
    // ncursestui
    NcursesTui();
    ~NcursesTui() override;

    private:
    bool hasNotStarted(); // utility which warns the user if they haven't initialized the tui. returns true if has not started
    std::vector<std::pair<Toluene::WindowId, WINDOW*>> windowPairs;
};