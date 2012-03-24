#-------------------------------------------------
#
# Project created by QtCreator 2011-08-12T12:21:47
#
#-------------------------------------------------

QT       += core gui

TARGET = PingWarning
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ping.cpp

HEADERS  += mainwindow.h \
    icmpdefs.h \
    ping.h

FORMS    += mainwindow.ui
LIBS += -lws2_32




