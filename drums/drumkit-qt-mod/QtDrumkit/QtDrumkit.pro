# Copyright (c) 2011 Nokia Corporation.
QT += core gui quick widgets multimedia
CONFIG += c++11


TEMPLATE = app
TARGET = drumkit
VERSION = 1.2


SOURCES += \
    main.cpp \
    drumengine.cpp \
    sampleplayer.cpp \
    toucheventfilter.cpp
#   touchevents.cpp

RESOURCES += samples.qrc \
        drumkit.qrc


HEADERS += \
        drumengine.h \
        audiointerface.h \
        sampleplayer.h \
    toucheventfilter.h
 #   touchevents.h


unix:!simulator {

    message(Desktop)
    target.path = /
    INSTALLS += target \

}



simulator {
   message(Simulator)
   CONFIG -= release
   CONFIG += debug

   # Use GameEnabler audio on simulator
   DEFINES += USE_GAMEENABLER
   SOURCES += audiogameenabler.cpp
   HEADERS += audiogameenabler.h

   platform_qml.source = qml/harmattan
   platform_qml.target = qml
   QML_IMPORT_PATH += qml/harmattan

}

UBUNTU_TOUCH {
    message("building for Ubuntu Touch")

    DEFINES += Q_OS_UBUNTU_TOUCH  \

    # figure out the current build architecture
    CLICK_ARCH=$$system(dpkg-architecture -qDEB_HOST_ARCH)

    # substitute the architecture in the manifest file
    QMAKE_SUBSTITUTES += $$PWD/qtc_packaging/ubuntu_touch/manifest.json.in
    manifest.files = qtc_packaging/ubuntu_touch/manifest.json
    manifest.path = /
    INSTALLS += manifest

    target.path = /
    click_files.path = /
    click_files.files = $$PWD/qtc_packaging/ubuntu_touch/*

    INSTALLS+=click_files
}

## The following is needed for the volume buttons to work.
#harmattan {
#   gameclassify.files += qtc_packaging/debian_harmattan/QtDrumkit.conf
#   gameclassify.path = /usr/share/policy/etc/syspart.conf.d
#   INSTALLS += gameclassify
#}




include(QtGameEnabler/qtgameenabler.pri)

# Put generated temp-files under tmp
#MOC_DIR = tmp
#OBJECTS_DIR = tmp
#RCC_DIR = tmp
#UI_DIR = tmp

#DEPLOYMENTFOLDERS = platform_qml common_qml

# Please do not modify the following two lines. Required for deployment.
#include(qmlapplicationviewer/qmlapplicationviewer.pri)
#qtcAddDeployment()

#OTHER_FILES += \
#    qtc_packaging/debian_harmattan/rules \
#    qtc_packaging/debian_harmattan/README \
#    qtc_packaging/debian_harmattan/copyright \
#    qtc_packaging/debian_harmattan/control \
#    qtc_packaging/debian_harmattan/compat \
#    qtc_packaging/debian_harmattan/changelog \
#    qtc_packaging/debian_harmattan/QtDrumkit.conf \
#    QtDrumkit_harmattan.desktop
