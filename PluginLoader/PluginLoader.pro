#-------------------------------------------------
#
# Project created by QtCreator 2011-09-27T00:09:49
#
#-------------------------------------------------
QT       += core gui
QT       -= network opengl sql svg xml xmlpatterns qt3support

CONFIG += qt x86
CONFIG -= x86_64 ppc64 ppc

macx:LIBS += -framework AppKit -framework Carbon

TARGET = PluginLoader
TEMPLATE = lib

DESTDIR = ../

SOURCES += caddins.cpp

HEADERS += caddins.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
