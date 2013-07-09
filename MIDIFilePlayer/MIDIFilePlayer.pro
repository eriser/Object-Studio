#-------------------------------------------------
#
# Project created by QtCreator 2011-09-27T18:07:53
#
#-------------------------------------------------

TARGET = MIDIFilePlayer
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

LIBS += -L../ -lMIDIFileReader
INCLUDEPATH += ../midifilereader

DEFINES += MIDIFILEPLAYER_LIBRARY

SOURCES += cmidifileplayer.cpp

HEADERS += cmidifileplayer.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE39AB982
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = MIDIFilePlayer.dll
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



