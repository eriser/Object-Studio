#-------------------------------------------------
#
# Project created by QtCreator 2011-09-30T15:23:26
#
#-------------------------------------------------

TARGET = Scope
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

INCLUDEPATH += ../WaveRecorder

SOURCES += ../WaveRecorder/qcanvas.cpp
HEADERS += ../WaveRecorder/qcanvas.h

DEFINES += SCOPE_LIBRARY

SOURCES += cscope.cpp \
    cscopeform.cpp \
    cscopecontrol.cpp \
    cspectrumcontrol.cpp

HEADERS += cscope.h \
    cscopeform.h \
    cscopecontrol.h \
    cspectrumcontrol.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEAFC5194
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = Scope.dll
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
    cscopeform.ui \
    cscopecontrol.ui \
    cspectrumcontrol.ui










