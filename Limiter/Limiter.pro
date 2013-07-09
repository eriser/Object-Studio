#-------------------------------------------------
#
# Project created by QtCreator 2011-10-07T14:19:54
#
#-------------------------------------------------

TARGET = Limiter
TEMPLATE = lib

DEFINES += LIMITER_LIBRARY

include(../SoftSynthsIncludes.pri)

SOURCES += climiter.cpp

HEADERS += climiter.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE9A8A7BC
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = Limiter.dll
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











