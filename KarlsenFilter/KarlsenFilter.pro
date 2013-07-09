#-------------------------------------------------
#
# Project created by QtCreator 2011-10-10T08:33:24
#
#-------------------------------------------------

TARGET = KarlsenFilter
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

DEFINES += KARLSENFILTER_LIBRARY

SOURCES += ckarlsenfilter.cpp

HEADERS += ckarlsenfilter.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEDC348C8
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = KarlsenFilter.dll
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

