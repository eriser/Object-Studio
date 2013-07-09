#-------------------------------------------------
#
# Project created by QtCreator 2011-09-26T09:57:16
#
#-------------------------------------------------

QT += core gui
QT -= network opengl sql svg xml xmlpatterns qt3support
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += qt x86
CONFIG -= x86_64 ppc64 ppc

TARGET = WaveBank
TEMPLATE = lib

LIBS += -L../ -lSoftSynthsClasses
INCLUDEPATH += ../SoftSynthsClasses
INCLUDEPATH += ../../ObjectComposerXML

DESTDIR = ../

SOURCES += cwavebank.cpp

HEADERS += cwavebank.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
