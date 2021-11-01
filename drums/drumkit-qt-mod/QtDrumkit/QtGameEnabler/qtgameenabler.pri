QT += multimedia

DEFINES -= GE_DEBUG

INCLUDEPATH += $$PWD/src

SOURCES += \
    $$PWD/src/audiobuffer.cpp \
    $$PWD/src/audiobufferplayinstance.cpp \
    $$PWD/src/audiomixer.cpp \
    $$PWD/src/pushaudioout.cpp \
    $$PWD/src/pullaudioout.cpp \
    $$PWD/src/audioeffect.cpp \
    $$PWD/src/audiosourceif.cpp


HEADERS  += \
    $$PWD/src/audiobuffer.h \
    $$PWD/src/audiobufferplayinstance.h \
    $$PWD/src/audiomixer.h \
    $$PWD/src/audioout.h \
    $$PWD/src/audiosourceif.h \
    $$PWD/src/pushaudioout.h \
    $$PWD/src/pullaudioout.h \
    $$PWD/src/geglobal.h \
    $$PWD/src/audioeffect.h \
    $$PWD/src/trace.h
