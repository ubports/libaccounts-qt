#!/bin/sh

export AG_APPLICATIONS=$TESTDIR
export AG_SERVICES=$TESTDIR
export AG_SERVICE_TYPES=$TESTDIR
export AG_PROVIDERS=$TESTDIR
export ACCOUNTS=/tmp/ 
export XDG_DATA_HOME=$TESTDIR

# If dbus-test-runner exists, use it to run the tests in a separate D-Bus
# session
if command -v dbus-test-runner > /dev/null ; then
	echo "Using dbus-test-runner"
	dbus-test-runner -m 180 -t ./accountstest
else
	echo "Using existing D-Bus session"
	$WRAPPER ./accountstest "$@"
fi
