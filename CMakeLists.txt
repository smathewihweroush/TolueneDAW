cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 20)
set(CXX_SCAN_FOR_MODULES OFF)
project(Toluene VERSION 0.1.0 LANGUAGES C CXX)

# Dependencies
# ncurses

set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)
find_package(Curses)
if (Curses_FOUND)
    add_library(Curses INTERFACE IMPORTED) # do we do this for less of a hassle?
    target_link_libraries(Curses INTERFACE "${CURSES_LIBRARIES}")
    target_include_directories(Curses INTERFACE "${CURSES_INCLUDE_DIRS}")
endif()

# Rt Audio

include(FetchContent)

FetchContent_Declare(
    rtaudio
    GIT_REPOSITORY https://github.com/thestk/rtaudio.git
    GIT_TAG master
)

FetchContent_MakeAvailable(rtaudio) # this is so easy in comparison to ncurses...

# Subdirectories

add_subdirectory(src)

# Executable

add_executable(Toluene)
target_sources(Toluene
    PRIVATE
        src/app/main.cpp
)

target_link_libraries(Toluene PRIVATE rtaudio)
target_link_libraries(Toluene PRIVATE Curses)
target_link_libraries(Toluene PRIVATE AudioBackend)
target_link_libraries(Toluene PRIVATE TUI)