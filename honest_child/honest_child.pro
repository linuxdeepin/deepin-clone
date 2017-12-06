TARGET = deepin-clone-honest
VERSION = 0.0.1
TEMPLATE = app
QT += widgets

DEFINES += QT_MESSAGELOGCONTEXT
DEFINES += HOST_ARCH_$$QMAKE_HOST.arch HOST_ARCH=\\\"$$QMAKE_HOST.arch\\\"

target.path = $$PREFIX/bin

INSTALLS += target

SOURCES += \
    main.cpp

include($$PWD/../common.pri)
