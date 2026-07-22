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

#include "config.h"
#include <iostream>

void Toluene::NamedConfig::addData(std::string name, std::string type, bool required) {
    if (hasData(name)) {
        std::cerr << "Config already has data with given name.\n";
        return;
    }
    datas.push_back({name, type, required});
}
void Toluene::NamedConfig::addEntry(std::string dataName, std::string value) {
    if (!hasData(dataName)) {
        std::cerr << "There is no data of the given name.\n";
        return;
    }
    entries[dataName] = value;
}
bool Toluene::NamedConfig::hasData(std::string name) {
    for (int i = 0; i < datas.size(); i++) {
        if (datas[i].name == name) return 1;
    }
    return 0;
}
std::string Toluene::NamedConfig::getType(std::string dataName) {
    for (int i = 0; i < datas.size(); i++) {
        if (datas[i].name == dataName) return datas[i].type;
    }
    std::cerr << "There is no data of the given name.\n";
    return "";
}
bool Toluene::NamedConfig::hasEntry(std::string dataName) {
    return entries.contains(dataName);
}
std::string Toluene::NamedConfig::getEntry(std::string dataName) {
    if (!hasData(dataName)) {
        std::cerr << "There is no data of the given name\n";
        return "";
    }
    return entries[dataName];
}
bool Toluene::NamedConfig::satisfied() {
    for (int i = 0; i < datas.size(); i++) {
        if (datas[i].required == true) {
            if (!hasEntry(datas[i].name)) return false;
        }
    }
    return true;
}