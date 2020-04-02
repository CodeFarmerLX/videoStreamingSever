#-------------------------------------------------
#
# Project created by QtCreator 2019-06-19T13:38:00
#
#-------------------------------------------------

QT       += core gui
QT       +=sql
QT       +=network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TubeStarServer
TEMPLATE = app


SOURCES += main.cpp\
        TubeServer.cpp \
    Usr.cpp \
    DBHelper.cpp \
    UsrControl.cpp \
    SocketHandler.cpp \
    NetWorkHelper.cpp \
    Room.cpp \
    RoomControl.cpp

HEADERS  += TubeServer.h \
    Usr.h \
    DBHelper.h \
    UsrControl.h \
    SocketHandler.h \
    NetWorkHelper.h \
    package.h \
    mysleep.h \
    Room.h \
    RoomControl.h \
    socketsleep.h

FORMS    += TubeServer.ui
