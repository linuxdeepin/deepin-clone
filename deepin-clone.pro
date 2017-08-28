QT += core core-private

TARGET = deepin-clone
VERSION = 0.0.1
TEMPLATE = app

isEmpty(DISABLE_GUI) {
    QT += widgets
    CONFIG += c++11 link_pkgconfig
    PKGCONFIG += dtkwidget
    DEFINES += ENABLE_GUI

    SOURCES += src/widgets/mainwindow.cpp

    HEADERS += src/widgets/mainwindow.h

    INCLUDEPATH += src/widgets

    icon.path = /usr/share/icons/hicolor/scalable/apps
    icon.files = $$PWD/$${TARGET}.svg

    desktop.path = /usr/share/applications
    desktop.files = $$PWD/$${TARGET}.desktop

    INSTALLS += icon desktop
} else {
    QT -= gui
}

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
    src/commandlineparser.cpp

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
    src/commandlineparser.h

INCLUDEPATH += src src/corelib

target.path = /usr/sbin

mimetype_xml.path = /usr/share/mime/packages
mimetype_xml.files = $$PWD/mimetype/$${TARGET}.xml

mimetype_dim_icon.path = /usr/share/icons/hicolor/scalable/mimetypes
mimetype_dim_icon.files = $$PWD/mimetype/application-x-deepinclone-dim.svg

INSTALLS += target mimetype_xml mimetype_dim_icon
