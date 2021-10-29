#-------------------------------------------------
#
# Project created by QtCreator 2011-09-30T21:11:32
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = midiball
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ball.cpp \
    midi_in.cpp

LIBS += -lasound

HEADERS  += mainwindow.h \
    ball.h \
    midi_in.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    logo_linuxideas.png \
    gpl-3.0.txt
