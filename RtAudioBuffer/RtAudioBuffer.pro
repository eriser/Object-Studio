#-------------------------------------------------
#
# Project created by QtCreator 2011-09-18T22:59:50
#
#-------------------------------------------------

QT       += core gui
QT       -= network opengl sql svg xml xmlpatterns qt3support
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += qt x86
CONFIG -= x86_64 ppc64 ppc

LIBS += -framework CoreMidi -framework CoreAudio -framework Carbon
LIBS += -L../ -lSoftSynthsClasses
LIBS += -L../ -lPluginLoader

macx:LIBS += -framework AppKit -framework Carbon

INCLUDEPATH += ../SoftSynthsClasses
INCLUDEPATH += ../../ObjectComposerXML
INCLUDEPATH += ../PluginLoader
INCLUDEPATH += ../WaveRecorder
INCLUDEPATH += ../MIDIFile2Wave
INCLUDEPATH += ../WaveGenerator

DEFINES += __MACOSX_CORE__

SOURCES += main.cpp\
        mainwindow.cpp \
    ../../../../../../Developer/Library/rtaudio-4.0.11/RtAudio.cpp \
    ../../../../../../Developer/Library/rtmidi-2.0.1/RtMidi.cpp \
    cdesktopcomponent.cpp \
    cdevicelist.cpp \
    cparameterscomponent.cpp \
    cknobcontrol.cpp \
    cdesktopcontainer.cpp \
    cpeakcontrol.cpp \
    cdbscale.cpp \
    cstereopeak.cpp \
    ../WaveRecorder/qcanvas.cpp \
    ../../ObjectComposerXML/qiphotorubberband.cpp \
    ../MIDIFile2Wave/EffectLabel.cpp \
    corebuffer.cpp \
    ../WaveGenerator/cwavefile.cpp \
    ../WaveGenerator/iwavefile.cpp \
    ../WaveGenerator/mp3lib/MP3Play.cpp \
    ../WaveGenerator/mp3lib/windowb.c \
    ../WaveGenerator/mp3lib/window.c \
    ../WaveGenerator/mp3lib/sbtb.c \
    ../WaveGenerator/mp3lib/sbt.c \
    ../WaveGenerator/mp3lib/mp3dec.c \
    ../WaveGenerator/mp3lib/l3side.c \
    ../WaveGenerator/mp3lib/l3sf.c \
    ../WaveGenerator/mp3lib/l3quant.c \
    ../WaveGenerator/mp3lib/l3msis.c \
    ../WaveGenerator/mp3lib/l3init.c \
    ../WaveGenerator/mp3lib/l3hybrid.c \
    ../WaveGenerator/mp3lib/l3huff.c \
    ../WaveGenerator/mp3lib/l3dec.c \
    ../WaveGenerator/mp3lib/l3alias.c \
    ../WaveGenerator/mp3lib/l2init.c \
    ../WaveGenerator/mp3lib/l2dec.c \
    ../WaveGenerator/mp3lib/l1init.c \
    ../WaveGenerator/mp3lib/l1dec.c \
    ../WaveGenerator/mp3lib/imdct.c \
    ../WaveGenerator/mp3lib/fdct.c \
    ../WaveGenerator/mp3lib/bstream.c \
    ../../ObjectComposerXML/mouseevents.cpp

HEADERS  += mainwindow.h \
    ../../../../../../Developer/Library/rtaudio-4.0.11/RtAudio.h \
    ../../../../../../Developer/Library/rtmidi-2.0.1/RtMidi.h \
    cdesktopcomponent.h \
    cdevicelist.h \
    cparameterscomponent.h \
    cknobcontrol.h \
    cdesktopcontainer.h \
    cpeakcontrol.h \
    cdbscale.h \
    cstereopeak.h \
    ../WaveRecorder/qcanvas.h \
    ../../ObjectComposerXML/qiphotorubberband.h \
    ../MIDIFile2Wave/EffectLabel.h \
    corebuffer.h \
    ../WaveGenerator/cwavefile.h \
    ../WaveGenerator/iwavefile.h \
    ../WaveGenerator/mp3lib/MP3Play.h \
    ../WaveGenerator/mp3lib/include/mp3dec.h \
    ../WaveGenerator/mp3lib/include/layer3.h \
    ../WaveGenerator/mp3lib/include/l3huff.h \
    ../WaveGenerator/mp3lib/include/bstream.h \
    ../WaveGenerator/mp3lib/debug.h \
    ../../ObjectComposerXML/mouseevents.h

FORMS    += mainwindow.ui \
    cdesktopcomponent.ui \
    cparameterscomponent.ui \
    cknobcontrol.ui \
    cdesktopcontainer.ui \
    cpeakcontrol.ui \
    cdbscale.ui \
    cstereopeak.ui

TARGET = objectstudio
TEMPLATE = app
DESTDIR    = ../

RESOURCES += \
    Resources.qrc





























































































