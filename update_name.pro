#-------------------------------------------------
#
# Project created by QtCreator 2014-10-22T08:59:58
#
#-------------------------------------------------

QT       += core
QT       += network

QT       -= gui

TARGET = update_name
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += c++11

TEMPLATE = app


SOURCES += main.cpp \
    updatename.cpp \
    userstream.cpp \
    settings.cpp \
    twitter.cpp \
    oauth.cpp \
    updatenamemain.cpp

HEADERS += \
    updatename.h \
    userstream.h \
    settings.h \
    update_name_oauth.h \
    twitter.h \
    oauth.h \
    authorize_update_name.h \
    updatenamemain.h

OTHER_FILES += \
    README.md
