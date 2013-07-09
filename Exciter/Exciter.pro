#-------------------------------------------------
#
# Project created by QtCreator 2011-10-07T15:30:26
#
#-------------------------------------------------

TARGET = Exciter
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

DEFINES += EXCITER_LIBRARY

SOURCES += cexciter.cpp

HEADERS += cexciter.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE9CCDB28
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = Exciter.dll
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

