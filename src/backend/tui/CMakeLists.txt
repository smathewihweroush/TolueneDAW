add_library(TUI)
target_sources(TUI
    PRIVATE
        tui.cpp
        ncursestui.cpp
    PUBLIC
        FILE_SET HEADERS
        FILES
            tui.h
            ncursestui.h
)
target_link_libraries(TUI PRIVATE Curses)