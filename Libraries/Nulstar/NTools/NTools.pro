# Base Settings
include($$TOP_SRCDIR/common.pri)

# Project Settings
QT -= gui
CONFIG += c++11 skip_target_version_ext
TEMPLATE = lib
TARGET = NTools
VERSION = $$NULSTAR_LIBRARIES_VERSION
DEFINES += NTOOLS_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

DESTDIR=$$LIBRARIES_OUTDIR

# Source Files

HEADERS += NTools.h \
    NLogger.h


# Dependencies

SOURCES += \
    NLogger.cpp
