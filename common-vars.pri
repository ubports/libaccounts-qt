#-----------------------------------------------------------------------------
# Common variables for all projects.
#-----------------------------------------------------------------------------
TOP_SRC_DIR     = $$PWD
TOP_BUILD_DIR   = $${TOP_SRC_DIR}/$(BUILD_DIR)
QMAKE_LIBDIR   += $${TOP_BUILD_DIR}/libaccounts-qt
DEFINES += BUILDING_ACCOUNTS_QT


#-----------------------------------------------------------------------------
# Project name (used e.g. in include file and doc install path).
# remember to update debian/* files if you changes this
#-----------------------------------------------------------------------------
PROJECT_NAME = accounts-qt


#-----------------------------------------------------------------------------
# Project version
# remember to update debian/* files if you changes this
#-----------------------------------------------------------------------------
PROJECT_VERSION = 0.33

#-----------------------------------------------------------------------------
# Library version
#-----------------------------------------------------------------------------
VERSION = 1.1.3


# End of File

