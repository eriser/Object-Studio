#-------------------------------------------------
#
# Project created by QtCreator 2011-09-26T09:33:35
#
#-------------------------------------------------

QT += core gui
QT -= network opengl sql svg xml xmlpatterns qt3support
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += qt x86
CONFIG -= x86_64 ppc64 ppc

TARGET = WaveGenerator
TEMPLATE = lib

LIBS += -L../ -lSoftSynthsClasses
INCLUDEPATH += ../SoftSynthsClasses

DESTDIR = ../

SOURCES += cwavegenerator.cpp \
    cwavefile.cpp \
    mp3lib/MP3Play.cpp \
    mp3lib/windowb.c \
    mp3lib/window.c \
    mp3lib/sbtb.c \
    mp3lib/sbt.c \
    mp3lib/mp3dec.c \
    mp3lib/l3side.c \
    mp3lib/l3sf.c \
    mp3lib/l3quant.c \
    mp3lib/l3msis.c \
    mp3lib/l3init.c \
    mp3lib/l3hybrid.c \
    mp3lib/l3huff.c \
    mp3lib/l3dec.c \
    mp3lib/l3alias.c \
    mp3lib/l2init.c \
    mp3lib/l2dec.c \
    mp3lib/l1init.c \
    mp3lib/l1dec.c \
    mp3lib/imdct.c \
    mp3lib/fdct.c \
    mp3lib/bstream.c \
    iwavefile.cpp

HEADERS += cwavegenerator.h \
    cwavefile.h \
    mp3lib/MP3Play.h \
    mp3lib/debug.h \
    mp3lib/include/mp3dec.h \
    mp3lib/include/layer3.h \
    mp3lib/include/l3huff.h \
    mp3lib/include/bstream.h \
    iwavefile.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}







