## [DEFAULT]
QT -= gui
QT += websockets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += Network.h \
           NWebSocketServer.h \
    NCoreService.h

SOURCES += NWebSocketServer.cpp \
    NCoreService.cpp

unix {
  target.path = /usr/lib
  INSTALLS += target
}

QMAKE_LFLAGS_RELEASE += -s
TEMPLATE = lib
TARGET = Network
DESTDIR = $$DESTDIR/Libraries
VERSION = 0.0.1.1
DEFINES += NETWORK_LIBRARY

