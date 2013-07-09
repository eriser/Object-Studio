#-------------------------------------------------
#
# Project created by QtCreator 2011-10-19T15:27:45
#
#-------------------------------------------------

TARGET = Equalizer
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

LIBS += -L../ -lWaveBank
INCLUDEPATH += ../wavebank

INCLUDEPATH += ../WaveRecorder

INCLUDEPATH += ../RtAudioBuffer
INCLUDEPATH += ../chorus

SOURCES += cequalizerframe.cpp \
    cequalizerform.cpp \
    ../RtAudioBuffer/cdbscale.cpp \
    cequalizergraph.cpp \
    ../WaveRecorder/qcanvas.cpp
HEADERS += cequalizerframe.h \
    cequalizerform.h \
    ../RtAudioBuffer/cdbscale.h \
    cequalizergraph.h \
    ../WaveRecorder/qcanvas.h

DEFINES += EQUALIZER_LIBRARY

SOURCES += cequalizer.cpp

HEADERS += cequalizer.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE9AF17CD
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = Equalizer.dll
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
    cequalizerframe.ui \
    cequalizerform.ui \
    ../RtAudioBuffer/cdbscale.ui \
    cequalizergraph.ui













