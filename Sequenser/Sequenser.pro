#-------------------------------------------------
#
# Project created by QtCreator 2011-10-07T09:35:25
#
#-------------------------------------------------

TARGET = Sequenser
TEMPLATE = lib

DEFINES += SEQUENSER_LIBRARY

include(../SoftSynthsIncludes.pri)

INCLUDEPATH += ../drummachine

SOURCES += csequenser.cpp
HEADERS += csequenser.h

SOURCES += \
    ../DrumMachine/sequenserclasses.cpp \
    ../DrumMachine/crepeatform.cpp \
    ../DrumMachine/cinsertpatternform.cpp \
    ../DrumMachine/cbeatframe.cpp \
    csequenserform.cpp

HEADERS += \
    ../DrumMachine/sequenserclasses.h \
    ../DrumMachine/crepeatform.h \
    ../DrumMachine/cinsertpatternform.h \
    ../DrumMachine/cbeatframe.h \
    csequenserform.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEAD8DA38
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = Sequenser.dll
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

FORMS += \
    ../DrumMachine/crepeatform.ui \
    ../DrumMachine/cinsertpatternform.ui \
    ../DrumMachine/cbeatframe.ui \
    csequenserform.ui















