#-------------------------------------------------
#
# Project created by QtCreator 2016-12-24T20:17:56
#
#-------------------------------------------------

QT       += core gui

QT += widgets serialport printsupport

TARGET = ZenQuad
TEMPLATE = app
CONFIG += warn_off

SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp


HEADERS  += mainwindow.h \
    qcustomplot.h


FORMS    += mainwindow.ui

