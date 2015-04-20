#-------------------------------------------------
#
# Project created by QtCreator 2014-09-16T08:07:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = untitled1
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    thread.cpp \
    meminfo.cpp \
    processos.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    thread.h \
    meminfo.h \
    processos.h

FORMS    += mainwindow.ui

QT += printsupport
