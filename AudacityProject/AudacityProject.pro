#-------------------------------------------------
#
# Project created by QtCreator 2011-10-20T09:35:48
#
#-------------------------------------------------

TARGET = AudacityProject
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

LIBS += -lWaveGenerator

INCLUDEPATH += ../wavegenerator

DEFINES += AUDACITYPROJECT_LIBRARY

SOURCES += caudacityproject.cpp

HEADERS += caudacityproject.h
