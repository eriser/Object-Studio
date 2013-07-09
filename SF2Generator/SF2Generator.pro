#-------------------------------------------------
#
# Project created by QtCreator 2011-09-26T11:10:10
#
#-------------------------------------------------

CONFIG += qt x86
CONFIG -= x86_64 ppc64 ppc

QT += core gui
QT -= network opengl sql svg xml xmlpatterns qt3support
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SF2Generator
TEMPLATE = lib

LIBS += -L../ -lSoftSynthsClasses
INCLUDEPATH += ../SoftSynthsClasses
INCLUDEPATH += ../../ObjectComposerXML

LIBS += -lWaveBank
INCLUDEPATH += ../wavebank

DESTDIR = ../

SOURCES += csf2generator.cpp \
    csf2file.cpp \
    enabler/win_mem.cpp \
    enabler/sfreader.cpp \
    enabler/sfnav.cpp \
    enabler/sflookup.cpp \
    enabler/sfdetect.cpp \
    enabler/riff.cpp \
    enabler/omega.cpp \
    enabler/hydra.cpp \
    enabler/enab.cpp

HEADERS += csf2generator.h \
    csf2file.h \
    enabler/win_mem.h \
    enabler/sfreader.h \
    enabler/sfnav.h \
    enabler/sflookup.h \
    enabler/sfenum.h \
    enabler/sfdetect.h \
    enabler/sfdata.h \
    enabler/riff.h \
    enabler/omega.h \
    enabler/hydra.h \
    enabler/enab.h \
    enabler/emuerrs.h \
    enabler/datatype.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}




