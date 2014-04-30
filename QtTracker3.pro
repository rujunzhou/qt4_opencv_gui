#-------------------------------------------------
#
# Project created by QtCreator 2014-04-27T11:07:42
#
#-------------------------------------------------

QT       += core gui

TARGET = QtTracker3
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

LIBS += `pkg-config opencv --libs --cflags`

