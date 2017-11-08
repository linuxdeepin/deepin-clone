TARGET = deepin-clone
VERSION = 0.0.1
TEMPLATE = app

CONFIG += link_pkgconfig
PKGCONFIG += dtkcore

DEFINES += QT_MESSAGELOGCONTEXT
DEFINES += HOST_ARCH_$$QMAKE_HOST.arch HOST_ARCH=\\\"$$QMAKE_HOST.arch\\\"

isEmpty(DISABLE_GUI) {
    include($$PWD/src/widgets/widgets.pri)
} else {
    QT -= gui
}

include($$PWD/src/corelib/corelib.pri)
include($$PWD/src/fixboot/fixboot.pri)

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

policy.path = /usr/share/polkit-1/actions
policy.files = com.deepin.pkexec.deepin-clone.policy

ionice.path = $${target.path}
ionice.files = deepin-clone-ionice

pkexec.path = /usr/bin
pkexec.files = deepin-clone-pkexec

dman.path = /usr/share/dman/
dman.files = $$PWD/dman/*

INSTALLS += target mimetype_xml mimetype_dim_icon policy ionice pkexec dman

TRANSLATIONS += $$PWD/translations/$${TARGET}.ts \
                $$PWD/translations/$${TARGET}_zh_CN.ts

!system(deepin-policy-ts-convert policy2ts com.deepin.pkexec.deepin-clone.policy.tmp translations): message("Failed policy to ts")
!system(deepin-policy-ts-convert ts2policy com.deepin.pkexec.deepin-clone.policy.tmp translations com.deepin.pkexec.deepin-clone.policy) {
    system(cp com.deepin.pkexec.deepin-clone.policy.tmp com.deepin.pkexec.deepin-clone.policy)
}

CONFIG(release, debug|release) {
    !system($$PWD/translate_generation.sh): error("Failed to generate translation")
}

translations.path = /usr/share/$${TARGET}/translations
translations.files = translations/*.qm

INSTALLS += translations
