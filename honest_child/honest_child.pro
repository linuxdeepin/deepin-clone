TARGET = deepin-clone-honest
VERSION = 0.0.1
TEMPLATE = app
QT += widgets

CONFIG += link_pkgconfig
PKGCONFIG += dtkwidget

DEFINES += QT_MESSAGELOGCONTEXT
DEFINES += HOST_ARCH_$$QMAKE_HOST.arch HOST_ARCH=\\\"$$QMAKE_HOST.arch\\\"

target.path = /usr/bin

INSTALLS += target

SOURCES += \
    main.cpp

include($$PWD/../common.pri)
