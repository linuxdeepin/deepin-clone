QT += core

HEADERS += \
    $$PWD/bootdoctor.h

SOURCES += \
    $$PWD/bootdoctor.cpp

RESOURCES += \
    $$PWD/scripts.qrc \
    $$PWD/repos.qrc

INCLUDEPATH += $$PWD

DEFINES += ENABLE_BOOTDOCTOR
