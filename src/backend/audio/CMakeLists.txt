add_library(AudioBackend STATIC) # must i do this
target_sources(AudioBackend
    PRIVATE
        audiobackend.cpp
        rtaudiobackend.cpp
    PUBLIC
        FILE_SET HEADERS
        FILES 
            audiobackend.h
            rtaudiobackend.h
)
target_link_libraries(AudioBackend PRIVATE rtaudio)