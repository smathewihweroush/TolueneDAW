add_library(Core)
target_sources(Core
    PRIVATE
        config.cpp
    PUBLIC
        FILE_SET HEADERS
        FILES
            config.h
)