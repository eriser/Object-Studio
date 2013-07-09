#-------------------------------------------------
#
# Project created by QtCreator 2011-10-03T17:36:25
#
#-------------------------------------------------

TARGET = MacroBox
TEMPLATE = lib

DEFINES += MACROBOX_LIBRARY

include(../SoftSynthsIncludes.pri)

LIBS += -L../ -lPluginLoader
macx:LIBS += -framework AppKit -framework Carbon

INCLUDEPATH += ../PluginLoader
INCLUDEPATH += ../RtAudioBuffer
INCLUDEPATH += ../MIDIFile2Wave

SOURCES += cmacrobox.cpp \
    cmacroboxform.cpp \
    ../RtAudioBuffer/cparameterscomponent.cpp \
    ../RtAudioBuffer/cknobcontrol.cpp \
    ../RtAudioBuffer/cjackscomponent.cpp \
    ../RtAudioBuffer/cdevicelist.cpp \
    ../RtAudioBuffer/cdesktopcontainer.cpp \
    ../RtAudioBuffer/cdesktopcomponent.cpp \
    ../../ObjectComposerXML/qiphotorubberband.cpp \
    ../MIDIFile2Wave/EffectLabel.cpp

HEADERS += cmacrobox.h \
    cmacroboxform.h \
    ../RtAudioBuffer/cparameterscomponent.h \
    ../RtAudioBuffer/cknobcontrol.h \
    ../RtAudioBuffer/cjackscomponent.h \
    ../RtAudioBuffer/cdevicelist.h \
    ../RtAudioBuffer/cdesktopcontainer.h \
    ../RtAudioBuffer/cdesktopcomponent.h \
    ../../ObjectComposerXML/qiphotorubberband.h \
    ../MIDIFile2Wave/EffectLabel.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE5A033D5
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = MacroBox.dll
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
    cmacroboxform.ui \
    ../RtAudioBuffer/cparameterscomponent.ui \
    ../RtAudioBuffer/cknobcontrol.ui \
    ../RtAudioBuffer/cjackscomponent.ui \
    ../RtAudioBuffer/cdesktopcontainer.ui \
    ../RtAudioBuffer/cdesktopcomponent.ui







