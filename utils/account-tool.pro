include( ../common-project-config.pri )
include( ../common-vars.pri )

TEMPLATE = app
VERSION   = $$PROJECT_VERSION
TARGET = account-tool
#CONFIG += debug
SOURCES += account-tool.cpp
HEADERS += account-tool.h
QT = core xml
CONFIG += console
CONFIG += link_pkgconfig

LIBS += -laccounts-qt
INCLUDEPATH += ../libaccounts-qt
PKGCONFIG += glib-2.0 \
             gobject-2.0 \
             libaccounts-glib

include( ../common-installs-config.pri )
