#-------------------------------------------------
#
# Project created by QtCreator 2011-10-04T23:16:49
#
#-------------------------------------------------

TARGET = DrumMachine
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

LIBS += -lWaveGenerator
INCLUDEPATH += ../wavegenerator

SOURCES += cdrummachine.cpp
HEADERS += cdrummachine.h

DEFINES += DRUMMACHINE_LIBRARY

SOURCES += \
    sequenserclasses.cpp \
    cdrummachineform.cpp \
    cbeatframe.cpp \
    crepeatform.cpp \
    cinsertpatternform.cpp

HEADERS += \
    sequenserclasses.h \
    cdrummachineform.h \
    cbeatframe.h \
    crepeatform.h \
    cinsertpatternform.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE59D262B
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = DrumMachine.dll
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

RESOURCES += \
    Sounds.qrc

FORMS += \
    cdrummachineform.ui \
    cbeatframe.ui \
    crepeatform.ui \
    cinsertpatternform.ui

























