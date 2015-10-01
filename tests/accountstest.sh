#!/bin/sh

# If dbus-test-runner exists, use it to run the tests in a separate D-Bus
# session
if command -v dbus-test-runner > /dev/null ; then
	echo "Using dbus-test-runner"
	dbus-test-runner -m 180 -t "${TESTDIR}/accountstest-wrapper.sh"
else
	echo "Using existing D-Bus session"
	"${TESTDIR}/accountstest-wrapper.sh"
fi
