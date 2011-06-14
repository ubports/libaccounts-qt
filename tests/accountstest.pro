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

#Check for the existence of aegis-crypto
system(pkg-config --exists aegis-crypto) :HAVE_AEGISCRYPTO=YES
contains(HAVE_AEGISCRYPTO, YES) {
    message("aegis-crypto detected...")
    DEFINES += HAVE_AEGISCRYPTO
}

include( ../common-installs-config.pri )

DATA_PATH = $${INSTALL_PREFIX}/share/libaccounts-qt-tests/

DEFINES += \
    SERVICES_DIR=\\\"$$DATA_PATH\\\" \
    SERVICE_TYPES_DIR=\\\"$$DATA_PATH\\\" \
    PROVIDERS_DIR=\\\"$$DATA_PATH\\\"

service.path = $$DATA_PATH
service.files += *.service
INSTALLS     += service

service-type.path = $$DATA_PATH
service-type.files += *.service-type
INSTALLS += service-type

provider.path = $$DATA_PATH
provider.files += *.provider
INSTALLS     += provider

testsuite.files = tests.xml
testsuite.path = $$DATA_PATH
INSTALLS += testsuite
