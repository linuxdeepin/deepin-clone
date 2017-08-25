QT += core core-private widgets

VERSION = 0.0.1

CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget

TARGET = deepin-clone

TEMPLATE = app

SOURCES += src/main.cpp \
    src/corelib/ddiskinfo.cpp \
    src/corelib/dpartinfo.cpp \
    src/corelib/ddevicediskinfo.cpp \
    src/corelib/ddevicepartinfo.cpp \
    src/corelib/dfilediskinfo.cpp \
    src/corelib/clonejob.cpp \
    src/corelib/helper.cpp \
    src/corelib/dvirtualimagefileengine.cpp \
    src/corelib/dvirtualimagefileio.cpp \
    src/corelib/dzlibiodevice.cpp \
    src/corelib/dzlibfile.cpp \
    src/commandlineparser.cpp \
    src/widgets/mainwindow.cpp

HEADERS += \
    src/corelib/ddiskinfo.h \
    src/corelib/dpartinfo.h \
    src/corelib/ddevicediskinfo.h \
    src/corelib/ddiskinfo_p.h \
    src/corelib/ddevicepartinfo.h \
    src/corelib/dpartinfo_p.h \
    src/corelib/dfilediskinfo.h \
    src/dglobal.h \
    src/corelib/clonejob.h \
    src/corelib/helper.h \
    src/corelib/dvirtualimagefileengine.h \
    src/corelib/dvirtualimagefileio.h \
    src/corelib/dzlibiodevice.h \
    src/corelib/dzlibfile.h \
    src/commandlineparser.h \
    src/widgets/mainwindow.h

INCLUDEPATH += src src/corelib src/widgets

target.path = /usr/sbin

INSTALLS += target
