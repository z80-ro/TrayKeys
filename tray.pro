#-------------------------------------------------
#
# Project created by QtCreator 2016-08-19T19:12:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tray
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    worker.cpp \
    transparent.cpp \
    worker_mouse.cpp \
    transparent_mouse.cpp

HEADERS  += mainwindow.h \
    worker.h \
    transparent.h \
    worker_mouse.h \
    transparent_mouse.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
