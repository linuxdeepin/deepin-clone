QT += core
QT -= gui

CONFIG += c++11

TARGET = deepin-ghost
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ddiskinfo.cpp \
    dpartinfo.cpp \
    ddevicediskinfo.cpp \
    ddevicepartinfo.cpp \
    dfilediskinfo.cpp \
    dfilepartinfo.cpp \
    clonejob.cpp \
    helper.cpp

HEADERS += \
    ddiskinfo.h \
    dpartinfo.h \
    ddevicediskinfo.h \
    ddiskinfo_p.h \
    ddevicepartinfo.h \
    dpartinfo_p.h \
    dfilediskinfo.h \
    dfilepartinfo.h \
    dglobal.h \
    clonejob.h \
    helper.h
