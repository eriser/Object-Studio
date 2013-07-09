#-------------------------------------------------
#
# Project created by QtCreator 2011-09-26T08:47:24
#
#-------------------------------------------------
QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

QT       += core gui
QT       -= network opengl sql svg xml xmlpatterns qt3support
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += qt x86
CONFIG -= x86_64 ppc64 ppc

INCLUDEPATH += ../../ObjectComposerXML

TARGET = SoftSynthsClasses
TEMPLATE = lib

DESTDIR = ../

SOURCES += \
    cmidibuffer.cpp \
    ../../ObjectComposerXML/qdomlite.cpp \
    caudiobuffer.cpp \
    ijack.cpp \
    idevice.cpp

HEADERS += softsynthsclasses.h \
    cmidibuffer.h \
    ../../ObjectComposerXML/qdomlite.h \
    idevicebase.h \
    ijackbase.h \
    caudiobuffer.h \
    ijack.h \
    ihost.h \
    idevice.h \
    cfastpointerlist.h \
    softsynthsdefines.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

HEADERS += \
    csounddevice.h

SOURCES += \
    csounddevice.cpp

HEADERS +=

SOURCES +=

HEADERS += \
    cfreqglider.h

SOURCES += \
    cfreqglider.cpp

HEADERS += \
    cpresets.h

SOURCES += \
    cpresets.cpp

HEADERS += \
    csoftsynthsform.h

SOURCES += \
    csoftsynthsform.cpp






