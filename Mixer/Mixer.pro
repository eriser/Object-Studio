#-------------------------------------------------
#
# Project created by QtCreator 2011-10-07T23:17:15
#
#-------------------------------------------------
TARGET = Mixer
TEMPLATE = lib

DEFINES += MIXER_LIBRARY

include(../SoftSynthsIncludes.pri)

INCLUDEPATH += ../RtAudioBuffer
INCLUDEPATH += ../WaveRecorder

SOURCES += cmixerframe.cpp \
    ../RtAudioBuffer/cstereopeak.cpp \
    ../RtAudioBuffer/cpeakcontrol.cpp \
    ../RtAudioBuffer/cdbscale.cpp \
    cmixerform.cpp \
    ../WaveRecorder/qcanvas.cpp
HEADERS += cmixerframe.h \
    ../RtAudioBuffer/cstereopeak.h \
    ../RtAudioBuffer/cpeakcontrol.h \
    ../RtAudioBuffer/cdbscale.h \
    cmixerform.h \
    ../WaveRecorder/qcanvas.h

SOURCES += cmixer.cpp

HEADERS += cmixer.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE929310E
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = Mixer.dll
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
    cmixerframe.ui \
    ../RtAudioBuffer/cstereopeak.ui \
    ../RtAudioBuffer/cpeakcontrol.ui \
    ../RtAudioBuffer/cdbscale.ui \
    cmixerform.ui









