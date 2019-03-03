#-------------------------------------------------
#
# Project created by QtCreator 2019-02-27T23:15:52
#
#-------------------------------------------------
QT       += serialport
QT       +=  printsupport
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cableTest
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    dataanalysetab.cpp \
    datarecordtab.cpp \
    qcustomplot.cpp

HEADERS  += widget.h \
    dataanalysetab.h \
    datarecordtab.h \
    qcustomplot.h
