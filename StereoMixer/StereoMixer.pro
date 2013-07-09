#-------------------------------------------------
#
# Project created by QtCreator 2013-01-21T00:36:07
#
#-------------------------------------------------
TARGET = StereoMixer
TEMPLATE = lib

DEFINES += STEREOMIXER_LIBRARY

include(../SoftSynthsIncludes.pri)

INCLUDEPATH += ../WaveRecorder
INCLUDEPATH += ../MIDIFile2Wave
INCLUDEPATH += ../RtAudioBuffer

SOURCES += ../RtAudioBuffer/cstereopeak.cpp \
    ../RtAudioBuffer/cpeakcontrol.cpp \
    ../RtAudioBuffer/cdbscale.cpp \
    cstereomixer.cpp \
    ../WaveRecorder/qcanvas.cpp \
    cstereomixerform.cpp \
    cstereochannelwidget.cpp \
    ../MIDIFile2Wave/EffectLabel.cpp \
    cmasterwidget.cpp
HEADERS += ../RtAudioBuffer/cstereopeak.h \
    ../RtAudioBuffer/cpeakcontrol.h \
    ../RtAudioBuffer/cdbscale.h \
    cstereomixer.h \
    ../WaveRecorder/qcanvas.h \
    cstereomixerform.h \
    cstereochannelwidget.h \
    ../MIDIFile2Wave/EffectLabel.h \
    cmasterwidget.h

SOURCES +=

HEADERS +=

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

FORMS += \
    ../RtAudioBuffer/cstereopeak.ui \
    ../RtAudioBuffer/cpeakcontrol.ui \
    ../RtAudioBuffer/cdbscale.ui \
    cstereomixerform.ui \
    cstereochannelwidget.ui \
    cmasterwidget.ui
