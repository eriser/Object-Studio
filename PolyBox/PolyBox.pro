#-------------------------------------------------
#
# Project created by QtCreator 2011-10-04T09:10:15
#
#-------------------------------------------------

TARGET = PolyBox
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

LIBS += -L../ -lPluginLoader
macx:LIBS += -framework AppKit -framework Carbon

INCLUDEPATH += ../PluginLoader
INCLUDEPATH += ../RtAudioBuffer
INCLUDEPATH += ../MacroBox
INCLUDEPATH += ../MIDI2CV
INCLUDEPATH += ../MIDIFile2Wave

SOURCES += ../MIDI2CV/ccvdevice.cpp \
    ../../ObjectComposerXML/qiphotorubberband.cpp \
    ../MIDIFile2Wave/EffectLabel.cpp
HEADERS += ../MIDI2CV/ccvdevice.h \
    ../../ObjectComposerXML/qiphotorubberband.h \
    ../MIDIFile2Wave/EffectLabel.h

DEFINES += POLYBOX_LIBRARY

SOURCES += cpolybox.cpp \
    ../RtAudioBuffer/cparameterscomponent.cpp \
    ../RtAudioBuffer/cknobcontrol.cpp \
    ../RtAudioBuffer/cjackscomponent.cpp \
    ../RtAudioBuffer/cdevicelist.cpp \
    ../RtAudioBuffer/cdesktopcontainer.cpp \
    ../RtAudioBuffer/cdesktopcomponent.cpp \
    ../MacroBox/cmacroboxform.cpp

HEADERS += cpolybox.h \
    ../RtAudioBuffer/cparameterscomponent.h \
    ../RtAudioBuffer/cknobcontrol.h \
    ../RtAudioBuffer/cjackscomponent.h \
    ../RtAudioBuffer/cdevicelist.h \
    ../RtAudioBuffer/cdesktopcontainer.h \
    ../RtAudioBuffer/cdesktopcomponent.h \
    ../MacroBox/cmacroboxform.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE64488FE
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = PolyBox.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

FORMS += \
    ../RtAudioBuffer/cparameterscomponent.ui \
    ../RtAudioBuffer/cknobcontrol.ui \
    ../RtAudioBuffer/cjackscomponent.ui \
    ../RtAudioBuffer/cdesktopcontainer.ui \
    ../RtAudioBuffer/cdesktopcomponent.ui \
    ../MacroBox/cmacroboxform.ui




