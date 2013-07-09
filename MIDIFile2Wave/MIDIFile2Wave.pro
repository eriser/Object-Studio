#-------------------------------------------------
#
# Project created by QtCreator 2013-02-24T11:21:48
#
#-------------------------------------------------
TARGET = MIDIFile2Wave
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += macextras

LIBS += -framework Cocoa -framework Carbon
macx:LIBS += -framework AppKit -framework Carbon  ## -framework CoreGraphics

DEFINES += MIDIFILE2WAVE_LIBRARY

INCLUDEPATH += ../RtAudioBuffer
INCLUDEPATH += ../WaveRecorder

INCLUDEPATH += ../StereoMixer
INCLUDEPATH += ../MIDIFilePlayer
INCLUDEPATH += ../SF2Player
INCLUDEPATH += ../SF2Generator
INCLUDEPATH += ../WaveBank
INCLUDEPATH += ../VSTHost
INCLUDEPATH += ../MIDIFileReader


SOURCES += ../RtAudioBuffer/cstereopeak.cpp \
    ../RtAudioBuffer/cpeakcontrol.cpp \
    ../RtAudioBuffer/cdbscale.cpp \
    cmidifile2wave.cpp \
    ../WaveRecorder/qcanvas.cpp
HEADERS += ../RtAudioBuffer/cstereopeak.h \
    ../RtAudioBuffer/cpeakcontrol.h \
    ../RtAudioBuffer/cdbscale.h \
    cmidifile2wave.h \
    ../WaveRecorder/qcanvas.h

HEADERS += \
    ../StereoMixer/cmasterwidget.h

SOURCES += \
    ../StereoMixer/cmasterwidget.cpp

FORMS += \
    ../StereoMixer/cmasterwidget.ui \
    ../RtAudioBuffer/cpeakcontrol.ui \
    ../RtAudioBuffer/cdbscale.ui

HEADERS += \
    EffectLabel.h \
    csf2channelwidget.h \
    cmixerwidget.h

SOURCES += \
    EffectLabel.cpp \
    csf2channelwidget.cpp \
    cmixerwidget.cpp

FORMS += \
    csf2channelwidget.ui \
    cmixerwidget.ui \
    ../RtAudioBuffer/cstereopeak.ui \

HEADERS += \
    ../SF2Player/csf2player.h

SOURCES += \
    ../SF2Player/csf2player.cpp

HEADERS += \
    ../SF2Player/csf2device.h

SOURCES += \
    ../SF2Player/csf2device.cpp

HEADERS += \
    ../StereoMixer/cstereomixer.h

SOURCES += \
    ../StereoMixer/cstereomixer.cpp

HEADERS += \
    ../VSTHost/cvsthost.h \
    ../MIDIFileReader/cmidifilereader.h \
    ../MIDIFilePlayer/cmidifileplayer.h

SOURCES += \
    ../VSTHost/cvsthost.cpp \
    ../MIDIFileReader/cmidifilereader.cpp \
    ../MIDIFilePlayer/cmidifileplayer.cpp

HEADERS += \
    ../SF2Generator/enabler/win_mem.h \
    ../SF2Generator/enabler/sfreader.h \
    ../SF2Generator/enabler/sfnav.h \
    ../SF2Generator/enabler/sflookup.h \
    ../SF2Generator/enabler/sfenum.h \
    ../SF2Generator/enabler/sfdetect.h \
    ../SF2Generator/enabler/sfdata.h \
    ../SF2Generator/enabler/riff.h \
    ../SF2Generator/enabler/omega.h \
    ../SF2Generator/enabler/hydra.h \
    ../SF2Generator/enabler/enab.h \
    ../SF2Generator/enabler/emuerrs.h \
    ../SF2Generator/enabler/datatype.h \
    ../SF2Generator/csf2generator.h

SOURCES += \
    ../SF2Generator/enabler/win_mem.cpp \
    ../SF2Generator/enabler/sfreader.cpp \
    ../SF2Generator/enabler/sfnav.cpp \
    ../SF2Generator/enabler/sflookup.cpp \
    ../SF2Generator/enabler/sfdetect.cpp \
    ../SF2Generator/enabler/riff.cpp \
    ../SF2Generator/enabler/omega.cpp \
    ../SF2Generator/enabler/hydra.cpp \
    ../SF2Generator/enabler/enab.cpp \
    ../SF2Generator/csf2generator.cpp

OBJECTIVE_SOURCES += \
    ../VSTHost/cmacwindow.mm

HEADERS += \
    ../VSTHost/cvsthostclass.h \
    ../VSTHost/cmacwindow.h

SOURCES += \
    ../VSTHost/cvsthostclass.cpp

HEADERS += \
    ../../ObjectComposerXML/mouseevents.h

SOURCES += \
    ../../ObjectComposerXML/mouseevents.cpp

HEADERS += \
    ../VSTHost/cvsthostform.h

SOURCES += \
    ../VSTHost/cvsthostform.cpp

HEADERS += \
    ../SF2Player/csf2playerform.h

SOURCES += \
    ../SF2Player/csf2playerform.cpp

FORMS += \
    ../VSTHost/cvsthostform.ui \
    ../SF2Player/csf2playerform.ui

HEADERS += \
    ../SF2Generator/csf2file.h

SOURCES += \
    ../SF2Generator/csf2file.cpp

HEADERS += \
    ../WaveBank/cwavebank.h

SOURCES += \
    ../WaveBank/cwavebank.cpp

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE5FF06FF
    TARGET.CAPABILITY =
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = MIDI2CV.dll
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

