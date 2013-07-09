QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

DEFINES += __MACOSX_CORE__

CONFIG += qt x86
CONFIG -= x86_64 ppc64 ppc

QT += core gui
QT -= network opengl sql svg xml xmlpatterns qt3support
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -L../ -lSoftSynthsClasses
INCLUDEPATH += ../SoftSynthsClasses
INCLUDEPATH += ../../ObjectComposerXML

DEFINES += devicename=\\\"$${TARGET}\\\"
DEFINES += deviceclass=C$${TARGET}
DEFINES += headerfile=\\\"c$${TARGET}.h\\\"

SOURCES += ../cdeviceclass.cpp
HEADERS += ../cdeviceclass.h
