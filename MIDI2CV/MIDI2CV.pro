#-------------------------------------------------
#
# Project created by QtCreator 2011-10-07T11:26:08
#
#-------------------------------------------------

TARGET = MIDI2CV
TEMPLATE = lib

DEFINES += MIDI2CV_LIBRARY

include(../SoftSynthsIncludes.pri)

SOURCES += ccvdevice.cpp
HEADERS += ccvdevice.h

SOURCES += cmidi2cv.cpp

HEADERS += cmidi2cv.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE5FF06FF
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = MIDI2CV.dll
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

