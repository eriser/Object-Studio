#-------------------------------------------------
#
# Project created by QtCreator 2011-10-10T10:18:14
#
#-------------------------------------------------
TARGET = Phaser
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

DEFINES += PHASER_LIBRARY

SOURCES += cphaser.cpp

HEADERS += cphaser.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE941E9BB
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = Phaser.dll
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

