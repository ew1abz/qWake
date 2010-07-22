# -------------------------------------------------
# Project created by QtCreator 2010-01-11T15:22:49
# -------------------------------------------------
#TARGET = bin/qWake
TARGET = qWake
TEMPLATE = app
VERSION = 0.0.1
RESOURCES = res/res.qrc
#OBJECTS_DIR = tmp
#UI_DIR = tmp
#MOC_DIR = tmp
#RCC_DIR = tmp
SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/wake.cpp \
    src/utils.cpp \
    src/hexspinbox.cpp \
    src/spinboxdelegate.cpp \
    src/hexlineeditdelegate.cpp
HEADERS += src/mainwindow.h \
    src/wake.h \
    src/utils.h \
    src/spinboxdelegate.h \
    src/hexspinbox.h \
    src/hexlineeditdelegate.h
FORMS += src/mainwindow.ui

# win32: HEADERS += src/comport_win.h
# win32: SOURCES += src/comport_win.cpp
# unix: HEADERS += src/comport_lin.h
# unix: SOURCES += src/comport_lin.cpp
#QMAKE_LIBDIR += ./qextserialport
QMAKE_LIBDIR += D:\Projects\qWake\qextserialport
LIBS += -lqextserialport
unix:DEFINES = _TTY_POSIX_
win32:DEFINES = _TTY_WIN_ QWT_DLL QT_DLL
