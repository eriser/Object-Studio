#-------------------------------------------------
#
# Project created by QtCreator 2011-10-10T10:11:53
#
#-------------------------------------------------
TARGET = NoiseGate
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

DEFINES += NOISEGATE_LIBRARY

SOURCES += cnoisegate.cpp

HEADERS += cnoisegate.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE649DF69
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = NoiseGate.dll
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

