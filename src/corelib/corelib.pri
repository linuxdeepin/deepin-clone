QT += core core-private

SOURCES += \
    $$PWD/ddiskinfo.cpp \
    $$PWD/dpartinfo.cpp \
    $$PWD/ddevicediskinfo.cpp \
    $$PWD/ddevicepartinfo.cpp \
    $$PWD/dfilediskinfo.cpp \
    $$PWD/clonejob.cpp \
    $$PWD/helper.cpp \
    $$PWD/dvirtualimagefileengine.cpp \
    $$PWD/dvirtualimagefileio.cpp \
    $$PWD/dzlibiodevice.cpp \
    $$PWD/dzlibfile.cpp

HEADERS += \
    $$PWD/ddiskinfo.h \
    $$PWD/dpartinfo.h \
    $$PWD/ddevicediskinfo.h \
    $$PWD/ddiskinfo_p.h \
    $$PWD/ddevicepartinfo.h \
    $$PWD/dpartinfo_p.h \
    $$PWD/dfilediskinfo.h \
    $$PWD/clonejob.h \
    $$PWD/helper.h \
    $$PWD/dvirtualimagefileengine.h \
    $$PWD/dvirtualimagefileio.h \
    $$PWD/dzlibiodevice.h \
    $$PWD/dzlibfile.h

INCLUDEPATH += $$PWD
