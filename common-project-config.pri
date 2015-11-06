#-----------------------------------------------------------------------------
# Common configuration for all projects.
#-----------------------------------------------------------------------------

QT             -= gui
CONFIG         += link_pkgconfig
MOC_DIR         = .moc
OBJECTS_DIR     = .obj
RCC_DIR         = resources
UI_DIR          = ui
UI_HEADERS_DIR  = ui/include
UI_SOURCES_DIR  = ui/src

!defined(TOP_SRC_DIR, var) {
    TOP_SRC_DIR = $$PWD
    TOP_BUILD_DIR = $${TOP_SRC_DIR}/$(BUILD_DIR)
}

# we don't like warnings...
#QMAKE_CXXFLAGS *= -Werror

CONFIG += c++11

#-----------------------------------------------------------------------------
# setup the installation prefix
#-----------------------------------------------------------------------------
INSTALL_PREFIX = /usr  # default installation prefix

# default prefix can be overriden by defining PREFIX when running qmake
isEmpty( PREFIX ) {
    message("====")
    message("==== NOTE: To override the installation path run: `qmake PREFIX=/custom/path'")
    message("==== (current installation path is `$${INSTALL_PREFIX}')")
} else {
    INSTALL_PREFIX = $${PREFIX}
    message("====")
    message("==== install prefix set to `$${INSTALL_PREFIX}'")
}

INSTALL_LIBDIR = $${INSTALL_PREFIX}/lib

# default library directory can be overriden by defining LIBDIR when
# running qmake
isEmpty( LIBDIR ) {
    message("====")
    message("==== NOTE: To override the library installation path run: `qmake LIBDIR=/custom/path'")
    message("==== (current installation path is `$${INSTALL_LIBDIR}')")
} else {
    INSTALL_LIBDIR = $${LIBDIR}
    message("====")
    message("==== library install path set to `$${INSTALL_LIBDIR}'")
}

isEmpty ( CMAKE_CONFIG_PATH ) {
    CMAKE_CONFIG_PATH = $${INSTALL_LIBDIR}/cmake
    message("====")
    message("==== NOTE: To override the cmake module installation path run: `qmake CMAKE_CONFIG_PATH=/custom/path'")
    message("==== (current installation path is `$${CMAKE_CONFIG_PATH}')")
} else {
    message("====")
    message("==== cmake module install path set to `$${CMAKE_CONFIG_PATH}'")
}
include( coverage.pri )

# End of File

