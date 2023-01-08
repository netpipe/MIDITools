QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets svg multimedia

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS #RTMIDI

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#ifdef RTMIDI
LIBS += -lrtmidi
#endif
LIBS += -lasound

INCLUDEPATH += ./qFluidlite/fluidlite/include
#./qFluidlite/fluidlite ./qFluidlite/

SOURCES += \
    keyboardmap.cpp \
    keylabel.cpp \
    main.cpp \
    mainwindow.cpp \
    pianokey.cpp \
    pianokeybd.cpp \
    pianopalette.cpp \
    pianoscene.cpp \
    ../qFluidlite/fluidlite/fluid_chan.c \
     ../qFluidlite/fluidlite/fluid_chorus.c \
     ../qFluidlite/fluidlite/fluid_conv.c \
     ../qFluidlite/fluidlite/fluid_defsfont.c \
     ../qFluidlite/fluidlite/fluid_dsp_float.c \
         ../qFluidlite/fluidlite/fluid_gen.c \
         ../qFluidlite/fluidlite/fluid_hash.c \
         ../qFluidlite/fluidlite/fluid_init.c \
         ../qFluidlite/fluidlite/fluid_list.c \
         ../qFluidlite/fluidlite/fluid_mod.c \
         ../qFluidlite/fluidlite/fluid_ramsfont.c \
         ../qFluidlite/fluidlite/fluid_rev.c \
         ../qFluidlite/fluidlite/fluid_settings.c \
         ../qFluidlite/fluidlite/fluid_synth.c \
         ../qFluidlite/fluidlite/fluid_sys.c \
         ../qFluidlite/fluidlite/fluid_tuning.c \
         ../qFluidlite/fluidlite/fluid_voice.c \
         ../qFluidlite/fluidlite/stb/stb_vorbis.c

HEADERS += \
    constants.h \
    keyboardmap.h \
    keylabel.h \
    mainwindow.h \
    pianodefs.h \
    pianokey.h \
    pianokeybd.h \
    pianopalette.h \
    pianoscene.h \
     ../qFluidlite/fluidlite/fluid_chan.h \
     ../qFluidlite/fluidlite/fluid_chorus.h \
     ../qFluidlite/fluidlite/fluid_config.h \
     ../qFluidlite/fluidlite/fluid_conv.h \
     ../qFluidlite/fluidlite/fluid_defsfont.h \
     ../qFluidlite/fluidlite/fluid_gen.h \
     ../qFluidlite/fluidlite/fluid_hash.h \
     ../qFluidlite/fluidlite/fluid_list.h \
     ../qFluidlite/fluidlite/fluid_midi.h \
     ../qFluidlite/fluidlite/fluid_mod.h \
     ../qFluidlite/fluidlite/fluid_phase.h \
     ../qFluidlite/fluidlite/fluid_ramsfont.h \
     ../qFluidlite/fluidlite/fluid_rev.h \
     ../qFluidlite/fluidlite/fluid_settings.h \
     ../qFluidlite/fluidlite/fluid_sfont.h \
     ../qFluidlite/fluidlite/fluid_sys.h \
     ../qFluidlite/fluidlite/fluid_tuning.h \
     ../qFluidlite/fluidlite/fluid_voice.h \
     ../qFluidlite/fluidlite/fluidsynth_priv.h \
     ../qFluidlite/fluidlite/include/fluidlite.h \
     ../qFluidlite/fluidlite/include/fluidsynth/gen.h \
     ../qFluidlite/fluidlite/include/fluidsynth/log.h \
     ../qFluidlite/fluidlite/include/fluidsynth/misc.h \
     ../qFluidlite/fluidlite/include/fluidsynth/mod.h \
     ../qFluidlite/fluidlite/include/fluidsynth/ramsfont.h \
     ../qFluidlite/fluidlite/include/fluidsynth/settings.h \
     ../qFluidlite/fluidlite/include/fluidsynth/sfont.h \
     ../qFluidlite/fluidlite/include/fluidsynth/synth.h \
     ../qFluidlite/fluidlite/include/fluidsynth/types.h \
     ../qFluidlite/fluidlite/include/fluidsynth/version.h \
     ../qFluidlite/fluidlite/include/fluidsynth/voice.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
