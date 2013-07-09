#-------------------------------------------------
#
# Project created by QtCreator 2011-10-25T19:27:32
#
#-------------------------------------------------

TARGET = PitchTracker
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

SOURCES += cpitchtrackerclass.cpp
HEADERS += cpitchtrackerclass.h

DEFINES += PITCHTRACKER_LIBRARY

SOURCES += cpitchtracker.cpp

HEADERS += cpitchtracker.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE0E9FB2A
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = PitchTracker.dll
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



