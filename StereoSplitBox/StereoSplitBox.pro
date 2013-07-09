#-------------------------------------------------
#
# Project created by QtCreator 2013-06-28T21:57:16
#
#-------------------------------------------------

TARGET = StereoSplitBox
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

DEFINES += STEREOSPLITBOX_LIBRARY

LIBS += -L../ -lPluginLoader
macx:LIBS += -framework AppKit -framework Carbon

INCLUDEPATH += ../MacroBox
INCLUDEPATH += ../RtAudioBuffer
INCLUDEPATH += ../PluginLoader
INCLUDEPATH += ../MIDIFile2Wave

SOURCES += cstereosplitbox.cpp \
    ../MacroBox/cmacroboxform.cpp \
    ../RtAudioBuffer/cparameterscomponent.cpp \
    ../RtAudioBuffer/cknobcontrol.cpp \
    ../RtAudioBuffer/cjackscomponent.cpp \
    ../RtAudioBuffer/cdevicelist.cpp \
    ../RtAudioBuffer/cdesktopcontainer.cpp \
    ../RtAudioBuffer/cdesktopcomponent.cpp \
    ../../ObjectComposerXML/qiphotorubberband.cpp \
    ../MIDIFile2Wave/EffectLabel.cpp

HEADERS += cstereosplitbox.h \
    ../MacroBox/cmacroboxform.h \
    ../RtAudioBuffer/cparameterscomponent.h \
    ../RtAudioBuffer/cknobcontrol.h \
    ../RtAudioBuffer/cjackscomponent.h \
    ../RtAudioBuffer/cdevicelist.h \
    ../RtAudioBuffer/cdesktopcontainer.h \
    ../RtAudioBuffer/cdesktopcomponent.h \
    ../../ObjectComposerXML/qiphotorubberband.h \
    ../MIDIFile2Wave/EffectLabel.h

FORMS += \
    ../MacroBox/cmacroboxform.ui \
    ../RtAudioBuffer/cparameterscomponent.ui \
    ../RtAudioBuffer/cknobcontrol.ui \
    ../RtAudioBuffer/cjackscomponent.ui \
    ../RtAudioBuffer/cdesktopcontainer.ui \
    ../RtAudioBuffer/cdesktopcomponent.ui
