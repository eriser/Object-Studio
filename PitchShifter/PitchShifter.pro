#-------------------------------------------------
#
# Project created by QtCreator 2011-10-08T11:54:28
#
#-------------------------------------------------

TARGET = PitchShifter
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

DEFINES += PITCHSHIFTER_LIBRARY

SOURCES += cpitchshifter.cpp

HEADERS += cpitchshifter.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE036C2D7
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = PitchShifter.dll
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

