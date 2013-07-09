#-------------------------------------------------
#
# Project created by QtCreator 2011-10-20T22:57:04
#
#-------------------------------------------------

TARGET = WaveRecorder
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

LIBS += -lWaveGenerator

INCLUDEPATH += ../wavegenerator

SOURCES += cwaverecorderform.cpp \
    cwavelanes.cpp \
    cwaveeditcontrol.cpp \
    cwaveeditwidget.cpp \
    qcanvas.cpp
HEADERS += cwaverecorderform.h \
    cwavelanes.h \
    cwaveeditcontrol.h \
    cwaveeditwidget.h \
    qcanvas.h

DEFINES += WAVERECORDER_LIBRARY

SOURCES += cwaverecorder.cpp

HEADERS += cwaverecorder.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE3E3FFD5
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = WaveRecorder.dll
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
    cwaverecorderform.ui \
    cwavelanes.ui \
    cwaveeditcontrol.ui \
    cwaveeditwidget.ui













