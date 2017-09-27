TARGET = deepin-clone
VERSION = 0.0.1
TEMPLATE = app

CONFIG += link_pkgconfig
PKGCONFIG += dtkcore

DEFINES += QT_MESSAGELOGCONTEXT

isEmpty(DISABLE_GUI) {
    include($$PWD/src/widgets/widgets.pri)
} else {
    QT -= gui
}

include($$PWD/src/corelib/corelib.pri)

SOURCES += src/main.cpp \
    src/commandlineparser.cpp

HEADERS += \
    src/dglobal.h \
    src/commandlineparser.h

INCLUDEPATH += src

target.path = /usr/sbin

mimetype_xml.path = /usr/share/mime/packages
mimetype_xml.files = $$PWD/mimetype/$${TARGET}.xml

mimetype_dim_icon.path = /usr/share/icons/hicolor/scalable/mimetypes
mimetype_dim_icon.files = $$PWD/mimetype/application-x-deepinclone-dim.svg

INSTALLS += target mimetype_xml mimetype_dim_icon

TRANSLATIONS += $$PWD/translations/$${TARGET}.ts \
                $$PWD/translations/$${TARGET}_zh_CN.ts

CONFIG(release, debug|release) {
    !system($$PWD/translate_generation.sh): error("Failed to generate translation")
}

translations.path = /usr/share/$${TARGET}/translations
translations.files = translations/*.qm

INSTALLS += translations
