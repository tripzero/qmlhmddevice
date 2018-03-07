TEMPLATE = lib
TARGET = hmddevice

CONFIG += qt plugin c++11 nostrip debug

LIBS += -lopenhmd

QT += qml quick 3dcore 3drender 3dinput

TARGET = $$qtLibraryTarget($$TARGET)

uri = openhmd

HEADERS += \
        hmddevice.h \
        hmddeviceplugin.h

SOURCES += \
        hmddevice.cpp \
        hmddeviceplugin.cpp

OTHER_FILES += qmldir

#Install plugin library, qmldir and types
qmldir.files = qmldir
types.files = hmddevice.types
unix {
	installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
	qmldir.path = $$installPath
        types.path = $$installPath
	target.path = $$installPath
        INSTALLS += target qmldir
}

QMAKE_CLEAN += *.so

DISTFILES += \
    hmddevice.types

