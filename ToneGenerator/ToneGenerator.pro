#-------------------------------------------------
#
# Project created by QtCreator 2011-09-27T21:36:26
#
#-------------------------------------------------

TARGET = ToneGenerator
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

LIBS += -L../ -lWaveBank
INCLUDEPATH += ../wavebank

DEFINES += TONEGENERATOR_LIBRARY

SOURCES += ctonegenerator.cpp

HEADERS += ctonegenerator.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE01DA696
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = ToneGenerator.dll
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

