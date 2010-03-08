include( common-vars.pri )

TEMPLATE  = subdirs
CONFIG   += ordered
SUBDIRS  += Accounts utils tests

include( common-installs-config.pri )
include( doc/doc.pri )

# End of File

