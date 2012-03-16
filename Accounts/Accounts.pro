
include( ../common-project-config.pri )
include( ../common-vars.pri )

TEMPLATE = lib

# Input
public_headers = \
    accountscommon.h \
    Manager manager.h \
    Account account.h \
    AccountService account-service.h \
    AuthData auth-data.h \
    Error error.h \
    Provider provider.h \
    Service service.h \
    ServiceType service-type.h

private_headers = \
    utils.h

HEADERS += \
    $$public_headers \
    $$private_headers

SOURCES += manager.cpp \
    account.cpp \
    account-service.cpp \
    auth-data.cpp \
    error.cpp \
    provider.cpp \
    service.cpp \
    service-type.cpp \
    utils.cpp

CONFIG += link_pkgconfig

DEFINES += QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII
QT += core xml
QT -= gui
TARGET = accounts-qt
PKGCONFIG += \
	glib-2.0 \
	gobject-2.0 \
	libaccounts-glib

QMAKE_CXXFLAGS += \
    -fno-exceptions \
    -fvisibility=hidden

headers.files = $$HEADERS

include( ../common-installs-config.pri )

# configuration feature
feature.files = accounts.prf
feature.path = $$[QT_INSTALL_DATA]/mkspecs/features
INSTALLS += feature

pkgconfig.files = accounts-qt.pc
pkgconfig.path = $${INSTALL_LIBDIR}/pkgconfig
INSTALLS += pkgconfig
