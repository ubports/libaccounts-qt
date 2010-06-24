
include( ../common-project-config.pri )
include( ../common-vars.pri )

TEMPLATE = lib

# Input
HEADERS += accountscommon.h \
    Manager manager.h \
    Account account.h \
    Provider provider.h \
    Service service.h \
    ServiceType service-type.h
SOURCES += manager.cpp \
    account.cpp \
    provider.cpp \
    service.cpp \
    service-type.cpp

CONFIG += link_pkgconfig

LIBS += -laccounts-glib
DEFINES += QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII
QT += core xml
QT -= gui
TARGET = accounts-qt
PKGCONFIG += glib-2.0 \
             gobject-2.0

QMAKE_CXXFLAGS = \
    -fno-exceptions \
    -fno-rtti \
    -fvisibility=hidden

headers.files = $$HEADERS

include( ../common-installs-config.pri )

# Temporary: install header files in /usr/include/accounts-qt too,
# while people are not #including <Accounts/*ClassName*>
tmp_headers.files = \
    accountscommon.h \
    manager.h \
    account.h \
    provider.h \
    service.h
tmp_headers.path = $${INSTALL_PREFIX}/include/$${TARGET}
INSTALLS += tmp_headers

# configuration feature
feature.files = accounts.prf
feature.path = $$[QT_INSTALL_DATA]/mkspecs/features
INSTALLS += feature

pkgconfig.files = accounts-qt.pc
pkgconfig.path = $${INSTALL_PREFIX}/lib/pkgconfig
INSTALLS += pkgconfig
