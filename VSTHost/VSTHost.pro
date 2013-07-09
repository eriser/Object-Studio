#-------------------------------------------------
#
# Project created by QtCreator 2011-10-10T11:29:08
#
#-------------------------------------------------

TARGET = VSTHost
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += macextras

LIBS += -framework Cocoa -framework Carbon
macx:LIBS += -framework AppKit -framework Carbon ## -framework CoreGraphics

SOURCES += cvsthostclass.cpp \
    cvsthostform.cpp \
    ../../ObjectComposerXML/mouseevents.cpp
OBJECTIVE_SOURCES += cmacwindow.mm
HEADERS += cvsthostclass.h \
    aeffectx.h \
    AEffect.h \
    cvsthostform.h \
    cmacwindow.h \
    ../../ObjectComposerXML/mouseevents.h

DEFINES += VSTHOST_LIBRARY

SOURCES += cvsthost.cpp

HEADERS += cvsthost.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEA8383B6
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = VSTHost.dll
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
    cvsthostform.ui
















