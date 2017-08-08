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
    dpartinfo.cpp

HEADERS += \
    ddiskinfo.h \
    util.h \
    dpartinfo.h
