# Copyright (c) 2011-2012 Nokia Corporation.

QT += core gui quick qml
CONFIG += mobility
MOBILITY = multimedia
TARGET = guitartuner
TEMPLATE = app
VERSION = 2.0

include(guitartunermodule/guitartunermodule.pri)

INCLUDEPATH += /usr/include/QtMultimediaKit \
                /usr/include/QtMobility/

LIBS += -lQtMultimediaKit

SOURCES += src/main.cpp

RESOURCES += guitartuner.qrc

OTHER_FILES += \
    qml/GuitarTunerPanel.qml \
    qml/IOControls.qml \
    qml/main-harmattan.qml \
    qml/main-symbian.qml \
    qml/Meter.qml \
    qml/Slider.qml \
    qml/StringIndicator.qml \
    qml/StringSelector.qml \
    qml/StringsModel.qml

symbian {
    TARGET = GuitarTuner
    TARGET.UID3 = 0xEC46D2C8
    TARGET.CAPABILITY += UserEnvironment
    ICON = icons/guitartuner.svg
    RESOURCES += rsc/symbian.qrc
}

contains(MEEGO_EDITION,harmattan) {
    DEFINES += MEEGO_EDITION_HARMATTAN
    RESOURCES += rsc/harmattan.qrc

    target.path = /opt/guitartuner/bin
    desktopfile.files = guitartuner.desktop
    desktopfile.path = /usr/share/applications
    icon.files = guitartuner.png
    icon.path = /usr/share/icons/hicolor/80x80/apps
    icon64.files = guitartuner.png
    icon64.path = /usr/share/icons/hicolor/64x64/apps

    INSTALLS += target desktopfile icon icon64
}

simulator {
    RESOURCES += rsc/symbian.qrc
}
