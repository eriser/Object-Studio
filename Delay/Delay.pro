#-------------------------------------------------
#
# Project created by QtCreator 2011-09-28T22:42:31
#
#-------------------------------------------------

TARGET = Delay
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

LIBS += -L../ -lWaveBank
INCLUDEPATH += ../wavebank

DEFINES += DELAY_LIBRARY

SOURCES += cdelay.cpp

HEADERS += cdelay.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEA491CF0
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = Delay.dll
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

