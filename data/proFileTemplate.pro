TEMPLATE  = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11
QT -= gui

include(openFrameworks-0.8.4.pri)

SOURCES +=  \
    main.cpp \
    ofApp.cpp

HEADERS += ofApp.h
