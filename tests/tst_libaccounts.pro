include( ../common-project-config.pri )
include( ../common-vars.pri )

TARGET = accountstest
SOURCES += \
    tst_libaccounts.cpp
QT = \
    core \
    testlib \
    xml

LIBS += -laccounts-qt5

INCLUDEPATH += $${TOP_SRC_DIR}
QMAKE_LIBDIR += \
    $${TOP_BUILD_DIR}/Accounts
QMAKE_RPATHDIR = $${QMAKE_LIBDIR}

#Check for the existence of aegis-crypto
system(pkg-config --exists aegis-crypto) :HAVE_AEGISCRYPTO=YES
contains(HAVE_AEGISCRYPTO, YES) {
    message("aegis-crypto detected...")
    DEFINES += HAVE_AEGISCRYPTO
}

include( ../common-installs-config.pri )

DATA_PATH = $${TOP_SRC_DIR}/tests

DEFINES += \
    DATA_PATH=\\\"$$DATA_PATH\\\"

QMAKE_EXTRA_TARGETS += check
check.depends = $${TARGET}
check.commands = "TESTDIR=$${TOP_SRC_DIR}/tests $${TOP_SRC_DIR}/tests/accountstest.sh"
