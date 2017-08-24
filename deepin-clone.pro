QT += core core-private widgets

VERSION = 0.0.1

CONFIG += c++11

TARGET = deepin-clone
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ddiskinfo.cpp \
    dpartinfo.cpp \
    ddevicediskinfo.cpp \
    ddevicepartinfo.cpp \
    dfilediskinfo.cpp \
    clonejob.cpp \
    helper.cpp \
    dvirtualimagefileengine.cpp \
    dvirtualimagefileio.cpp \
    dzlibiodevice.cpp \
    dzlibfile.cpp \
    commandlineparser.cpp

HEADERS += \
    ddiskinfo.h \
    dpartinfo.h \
    ddevicediskinfo.h \
    ddiskinfo_p.h \
    ddevicepartinfo.h \
    dpartinfo_p.h \
    dfilediskinfo.h \
    dglobal.h \
    clonejob.h \
    helper.h \
    dvirtualimagefileengine.h \
    dvirtualimagefileio.h \
    dzlibiodevice.h \
    dzlibfile.h \
    commandlineparser.h
