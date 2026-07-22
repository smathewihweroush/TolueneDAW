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

#include <map>
#include <string>
#include <vector>

namespace Toluene {
    struct NamedData { // data with string representations for stored value and type
        std::string name; // the name of the data
        std::string type; // the type of the data
        bool required; // does the data have to be provided?
    };
    class NamedConfig { // config which uses primarily strings to store data
        public:
        // add data with a name and type. required by default.
        void addData(std::string name, std::string type, bool required = true);
        // add an entry for some data.
        void addEntry(std::string dataName, std::string value);
        // checks if theres some data
        bool hasData(std::string name);
        // gets the type of a data
        std::string getType(std::string dataName);
        // checks if theres an entry for some data
        bool hasEntry(std::string dataName);
        // gets the entry for some data. if no entry, return empty string.
        std::string getEntry(std::string dataName);
        // checks if config has all necessary data filled
        bool satisfied();

        private:
        std::vector<NamedData> datas; // all data that will be parsed in the config
        std::map<std::string, std::string> entries; // all data that is stored
    };
}