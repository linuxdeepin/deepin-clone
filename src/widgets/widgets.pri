QT += core widgets
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget
DEFINES += ENABLE_GUI

SOURCES += \
    $$PWD/mainwindow.cpp \
    $$PWD/viewbox.cpp \
    $$PWD/contentpairpage.cpp \
    $$PWD/selectactionpage.cpp \
    $$PWD/iconlabel.cpp \
    $$PWD/utilitylist.cpp \
    $$PWD/utilitylistitem.cpp

HEADERS += \
    $$PWD/mainwindow.h \
    $$PWD/viewbox.h \
    $$PWD/contentpairpage.h \
    $$PWD/selectactionpage.h \
    $$PWD/iconlabel.h \
    $$PWD/utilitylist.h \
    $$PWD/utilitylistitem.h

INCLUDEPATH += $$PWD

icon.path = /usr/share/icons/hicolor/scalable/apps
icon.files = $$_PRO_FILE_PWD_/$${TARGET}.svg

desktop.path = /usr/share/applications
desktop.files = $$_PRO_FILE_PWD_/$${TARGET}.desktop

INSTALLS += icon desktop

RESOURCES += \
    $$PWD/../../icons.qrc \
    $$PWD/qss.qrc
