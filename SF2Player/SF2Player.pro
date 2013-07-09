#-------------------------------------------------
#
# Project created by QtCreator 2011-10-06T15:24:18
#
#-------------------------------------------------

TARGET = SF2Player
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

LIBS += -lSF2Generator
LIBS += -lWaveBank

INCLUDEPATH += ../wavebank
INCLUDEPATH += ../sf2generator

SOURCES += csf2player.cpp
HEADERS += csf2player.h

DEFINES += SF2PLAYER_LIBRARY

SOURCES += \
    csf2playerform.cpp

HEADERS += \
    csf2playerform.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEBE8270E
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = SF2Player.dll
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

FORMS += \
    csf2playerform.ui

HEADERS += \
    csf2device.h

SOURCES += \
    csf2device.cpp












