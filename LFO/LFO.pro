#-------------------------------------------------
#
# Project created by QtCreator 2011-09-28T22:53:14
#
#-------------------------------------------------

TARGET = LFO
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

LIBS += -L../ -lWaveBank
INCLUDEPATH += ../wavebank

DEFINES += LFO_LIBRARY

SOURCES += clfo.cpp

HEADERS += clfo.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE41622DF
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = LFO.dll
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

