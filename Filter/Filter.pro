#-------------------------------------------------
#
# Project created by QtCreator 2011-09-28T23:03:13
#
#-------------------------------------------------

TARGET = Filter
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

DEFINES += FILTER_LIBRARY

SOURCES += cfilter.cpp

HEADERS += cfilter.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE9BF8F87
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = Filter.dll
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

