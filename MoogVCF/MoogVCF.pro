#-------------------------------------------------
#
# Project created by QtCreator 2011-10-10T08:53:34
#
#-------------------------------------------------

TARGET = MoogVCF
TEMPLATE = lib

include(../SoftSynthsIncludes.pri)

DEFINES += MOOGVCF_LIBRARY

SOURCES += cmoogvcf.cpp

HEADERS += cmoogvcf.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE66802F9
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = MoogVCF.dll
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

