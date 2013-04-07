# Copyright (c) 2012 Nokia Corporation.

CONFIG += qt plugin

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/constants.h \
    $$PWD/fastfouriertransformer.h \
    $$PWD/guitartuner.h \
    $$PWD/guitartunerplugin.h \
    $$PWD/voiceanalyzer.h \
    $$PWD/voicegenerator.h

SOURCES += \
    $$PWD/fastfouriertransformer.cpp \
    $$PWD/fftpack.c \
    $$PWD/guitartuner.cpp \
    $$PWD/guitartunerplugin.cpp \
    $$PWD/voiceanalyzer.cpp \
    $$PWD/voicegenerator.cpp

qmldir.files = qmldir
qmldir.path = $$[QT_INSTALL_IMPORTS]/guitartuner
INSTALLS += qmldir
