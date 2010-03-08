include( ../common-project-config.pri )
include( ../common-vars.pri )

QMAKE_EXTRA_TARGETS += check
check.commands = ./accountstest

TEMPLATE = subdirs
SUBDIRS = accountstest.pro
