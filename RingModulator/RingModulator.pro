#-------------------------------------------------
#
# Project created by QtCreator 2011-10-07T11:47:16
#
#-------------------------------------------------

TARGET = RingModulator
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

DEFINES += RINGMODULATOR_LIBRARY

SOURCES += cringmodulator.cpp

HEADERS += cringmodulator.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE0F3371C
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = RingModulator.dll
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

