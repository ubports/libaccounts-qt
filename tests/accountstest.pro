include( ../common-project-config.pri )
include( ../common-vars.pri )

TARGET = accountstest
CONFIG += qtestlib
SOURCES += \
    accountstest.cpp
HEADERS += \
    accountstest.h
QT = core xml

LIBS += -laccounts-qt
INCLUDEPATH += $${TOP_SRC_DIR}/libaccounts-qt

include( ../common-installs-config.pri )

DATA_PATH = $${INSTALL_PREFIX}/share/libaccounts-qt-tests/

DEFINES += \
    SERVICES_DIR=\\\"$$DATA_PATH\\\" \
    PROVIDERS_DIR=\\\"$$DATA_PATH\\\"

service.path = $$DATA_PATH
service.files += *.service
INSTALLS     += service

provider.path = $$DATA_PATH
provider.files += *.provider
INSTALLS     += provider

testsuite.files = tests.xml
testsuite.path = $$DATA_PATH
INSTALLS += testsuite
