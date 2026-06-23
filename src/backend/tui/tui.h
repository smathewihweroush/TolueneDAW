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

#include <memory>
#include <ncurses.h>
#include <vector>

namespace Toluene {
    typedef unsigned int WindowId; // an id for a window
    
    class Tui;
    class Window { // contains information for a window
        public:
        WindowId id; // the id of a window instance
        int index; // the position of the window, lower index windows get drawn first (therefore being in the background)
        bool visible; // is the window visible?
        Tui* tui; // tui instance which controls this window 

        void draw() {};
    };

    class Tui {
        public:
        virtual void initialize(bool needsColor) = 0; // set up screen, main window, and any backends
        virtual void addWindow() = 0;

        std::vector<std::shared_ptr<Window>> windows;
    };
}

// main()create new win