#-------------------------------------------------
#
# Project created by QtCreator 2016-01-10T23:14:51
#
#-------------------------------------------------

QT       += core gui
QT       += network

QMAKE_CXXFLAGS += -fshort-wchar

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = otdviewer
TEMPLATE = app


SOURCES += main.cpp\
        otdviewermain.cpp \

HEADERS  += otdviewermain.h

FORMS    += otdviewermain.ui

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../lib/release/ -lgk-lib-1.0
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../lib/debug/ -lgk-lib-1.0
#else:
unix: LIBS += -L$$PWD/../../lib/ -lgk-lib-1.0

INCLUDEPATH += $$PWD/../../include
INCLUDEPATH += $$PWD/../../inc
DEPENDPATH += $$PWD/../../include

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../lib/release/libgk-lib-1.0.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../lib/debug/libgk-lib-1.0.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../lib/release/gk-lib-1.0.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../lib/debug/gk-lib-1.0.lib
#else:
unix: PRE_TARGETDEPS += $$PWD/../../lib/libgk-lib-1.0.a
