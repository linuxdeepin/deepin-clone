QT += core
QT -= gui

CONFIG += c++11

TARGET = deepin-ghost
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ddiskinfo.cpp \
    util.cpp \
    dpartinfo.cpp \
    ddevicediskinfo.cpp \
    ddevicepartinfo.cpp \
    dfilediskinfo.cpp \
    dfilepartinfo.cpp

HEADERS += \
    ddiskinfo.h \
    util.h \
    dpartinfo.h \
    ddevicediskinfo.h \
    ddiskinfo_p.h \
    ddevicepartinfo.h \
    dpartinfo_p.h \
    dfilediskinfo.h \
    dfilepartinfo.h \
    dglobal.h
