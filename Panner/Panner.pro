#-------------------------------------------------
#
# Project created by QtCreator 2011-09-29T11:59:21
#
#-------------------------------------------------
TARGET = Panner
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

DEFINES += PANNER_LIBRARY

SOURCES += cpanner.cpp

HEADERS += cpanner.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEA8CBBBB
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = Panner.dll
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

