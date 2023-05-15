#-------------------------------------------------
#
# Project created by QtCreator 2018-02-18T15:54:59
#
#-------------------------------------------------

QT       += core gui xml serialport uitools network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++2a -Wunused-parameter -Werror

DEFINES += QT_DEPRECATED_WARNINGS

TARGET = BenchSoft
TEMPLATE = app
VERSION = 1.0.6

CONFIG(debug, debug|release) {
    DESTDIR = $OUT_PWD/../../MB_Debug
} else {
    DESTDIR = $OUT_PWD/../../MB_Release
}
MOC_DIR = ../common/build/moc
RCC_DIR = ../common/build/rcc
UI_DIR = ../common/build/ui

OTHER_FILES += icon.ico

unix:OBJECTS_DIR = ../common/build/o/unix
win32:OBJECTS_DIR = ../common/build/o/win
macx:OBJECTS_DIR = ../common/build/o/mac

conf.input = DDBconfig.xml.in
CONFIG(debug, debug|release) {
    conf.output = $OUT_PWD/../../MB_Debug/DDBconfig.xml
} else {
    conf.output = $OUT_PWD/../../MB_Release/DDBconfig.xml
}
QMAKE_SUBSTITUTES += conf

CONFIG(release, debug|release) {
    QMAKE_POST_LINK = $$(QTDIR)/bin/windeployqt $$OUT_PWD/../MB_Release
}

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

DISTFILES += \
    DDBconfig.xml.in
