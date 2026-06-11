add_library(AudioBackend STATIC) # must i do this
target_sources(AudioBackend
    PRIVATE
        audiobackend.cpp
    PUBLIC
        FILE_SET HEADERS
        FILES audiobackend.h
)
target_link_libraries(AudioBackend PRIVATE rtaudio)