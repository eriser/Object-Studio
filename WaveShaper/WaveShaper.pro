#-------------------------------------------------
#
# Project created by QtCreator 2011-10-25T20:17:53
#
#-------------------------------------------------

TARGET = WaveShaper
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

DEFINES += WAVESHAPER_LIBRARY

SOURCES += cwaveshaper.cpp

HEADERS += cwaveshaper.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE4A5BEF9
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = WaveShaper.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

