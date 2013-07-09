#-------------------------------------------------
#
# Project created by QtCreator 2011-10-25T20:07:59
#
#-------------------------------------------------

TARGET = Unifilter
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

DEFINES += UNIFILTER_LIBRARY

SOURCES += cunifilter.cpp

HEADERS += cunifilter.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE91266A9
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = Unifilter.dll
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

