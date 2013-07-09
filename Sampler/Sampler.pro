#-------------------------------------------------
#
# Project created by QtCreator 2011-10-25T22:20:57
#
#-------------------------------------------------

TARGET = Sampler
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

LIBS += -lWaveGenerator
LIBS += -L../ -lWaveBank

INCLUDEPATH += ../wavegenerator
INCLUDEPATH += ../wavebank
INCLUDEPATH += ../WaveRecorder

INCLUDEPATH += ../envelope
INCLUDEPATH += ../WaveRecorder
INCLUDEPATH += ../pitchtracker

SOURCES += csamplerform.cpp \
    cwavelayers.cpp \
    ../Envelope/cadsrwidget.cpp \
    ../Envelope/cadsrcontrol.cpp \
    ../Envelope/cadsr.cpp \
    ckeylayoutcontrol.cpp \
    clayer.cpp \
    crange.cpp \
    ../PitchTracker/cpitchtrackerclass.cpp \
    ../WaveRecorder/cwaveeditwidget.cpp \
    ../WaveRecorder/cwaveeditcontrol.cpp \
    ckeyrangescontrol.cpp \
    csamplergenerator.cpp \
    csamplerdevice.cpp \
    ckeylayerscontrol.cpp \
    ../WaveRecorder/qcanvas.cpp \
    cmidinoteedit.cpp
HEADERS += csamplerform.h \
    cwavelayers.h \
    ../Envelope/cadsrwidget.h \
    ../Envelope/cadsrcontrol.h \
    ../Envelope/cadsr.h \
    ckeylayoutcontrol.h \
    clayer.h \
    crange.h \
    ../PitchTracker/cpitchtrackerclass.h \
    ../WaveRecorder/cwaveeditwidget.h \
    ../WaveRecorder/cwaveeditcontrol.h \
    ckeyrangescontrol.h \
    csamplergenerator.h \
    csamplerdevice.h \
    ckeylayerscontrol.h \
    ../WaveRecorder/qcanvas.h \
    cmidinoteedit.h

DEFINES += SAMPLER_LIBRARY

SOURCES += csampler.cpp

HEADERS += csampler.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEA56FDE0
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = Sampler.dll
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
    csamplerform.ui \
    cwavelayers.ui \
    ../Envelope/cadsrwidget.ui \
    ../Envelope/cadsrcontrol.ui \
    ckeylayoutcontrol.ui \
    ../WaveRecorder/cwaveeditwidget.ui \
    ../WaveRecorder/cwaveeditcontrol.ui \
    ckeyrangescontrol.ui \
    ckeylayerscontrol.ui \
    cmidinoteedit.ui

HEADERS += \
    ../MIDI2CV/ccvdevice.h

SOURCES += \
    ../MIDI2CV/ccvdevice.cpp

RESOURCES += \
    Resources.qrc





























