QT       += core gui widgets

TARGET = dfm-plugin-dim-file
TEMPLATE = lib
CONFIG += plugin link_pkgconfig
PKGCONFIG += dde-file-manager

DISTFILES += dfm-plugin-dim-file.json

SOURCES += \
    main.cpp \
    dimfilecontroller.cpp \
    dimfileinfo.cpp

HEADERS += \
    dimfilecontroller.h \
    dimfileinfo.h

include($$PWD/../app/src/corelib/corelib.pri)

INCLUDEPATH += $$PWD/../app/src

isEmpty(LIB_INSTALL_DIR)
    LIB_INSTALL_DIR = $${INSTALL_ROOT}/usr/lib/$${QMAKE_HOST.arch}-linux-gnu

isEmpty(DFM_PLUGIN_DIR)
    DFM_PLUGIN_DIR = $${LIB_INSTALL_DIR}/dde-file-manager/plugins/controllers

target.path = $$DFM_PLUGIN_DIR

INSTALLS += target
