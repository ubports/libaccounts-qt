
include( ../common-project-config.pri )
include( ../common-vars.pri )

TEMPLATE = lib

# Input
public_headers = \
    accountscommon.h \
    Manager manager.h \
    Account account.h \
    AccountService account-service.h \
    Application application.h \
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
    application.cpp \
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

greaterThan(QT_MAJOR_VERSION, 4) {
    TARGET = accounts-qt5
} else {
    TARGET = accounts-qt
}

PKGCONFIG += \
	glib-2.0 \
	gobject-2.0 \
	libaccounts-glib

QMAKE_CXXFLAGS += \
    -fno-exceptions \
    -fvisibility=hidden

headers.files = $$HEADERS

include( ../common-installs-config.pri )

pkgconfig.files = $${TOP_BUILD_DIR}/$${TARGET}.pc
include($${TOP_SRC_DIR}/common-pkgconfig.pri)
INSTALLS += pkgconfig
