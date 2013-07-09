#-------------------------------------------------
#
# Project created by QtCreator 2011-09-26T11:23:29
#
#-------------------------------------------------
QT       += core gui
QT       -= network opengl sql svg xml xmlpatterns qt3support

CONFIG += qt x86
CONFIG -= x86_64 ppc64 ppc

TARGET = MIDIFileReader
TEMPLATE = lib

DESTDIR = ../

SOURCES += cmidifilereader.cpp

HEADERS += cmidifilereader.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
