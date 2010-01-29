# -------------------------------------------------
# Project created by QtCreator 2010-01-11T15:22:49
# -------------------------------------------------
TARGET = bin/qWake
TEMPLATE = app
VERSION = 0.0.1

RESOURCES = res/res.qrc
OBJECTS_DIR = tmp
UI_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/wake.cpp \
    src/rs_io.cpp \
    src/i2c_adapter.cpp \
    src/hexspinbox.cpp
HEADERS += src/mainwindow.h \
    src/wake.h \
    src/rs_io.h \
    src/i2c_adapter.h \
    src/hexspinbox.h
FORMS += src/mainwindow.ui
