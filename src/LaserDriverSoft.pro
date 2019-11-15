#-------------------------------------------------
#
# Project created by QtCreator 2018-02-18T15:54:59
#
#-------------------------------------------------

QT       += core gui xml serialport uitools network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LaserDriverSoft
TEMPLATE = app
VERSION = 1.0.4


SOURCES += main.cpp\
        mainwindow.cpp \
    comport.cpp \
    consolelogger.cpp \
    xmlreader.cpp \
    aboutdialog.cpp \
    filedownloader.cpp \
    bitslayout.cpp \
    changelimitsdialog.cpp \
    aboutdevicedialog.cpp \
    appsettings.cpp \
    calibratedialog.cpp \
    selectdevicedialog.cpp \
    devicelimit.cpp \
    command.cpp \
    parametercontroller.cpp \
    signedcommand.cpp

HEADERS  += mainwindow.h \
    commondefines.h \
    comport.h \
    consolelogger.h \
    xmlreader.h \
    aboutdialog.h \
    filedownloader.h \
    globals.h \
    bitslayout.h \
    changelimitsdialog.h \
    aboutdevicedialog.h \
    appsettings.h \
    calibratedialog.h \
    selectdevicedialog.h \
    devicelimit.h \
    command.h \
    parametercontroller.h \
    signedcommand.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    parambigwidget.ui \
    paramcompactwidget.ui \
    changelimitsdialog.ui \
    paramtextwidget.ui \
    aboutdevicedialog.ui \
    calibratedialog.ui \
    selectdevicedialog.ui

RESOURCES += \
    resources.qrc

#DISTFILES += \
#    DDBconfig.xml

RC_ICONS = icon.ico
