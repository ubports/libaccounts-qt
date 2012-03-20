#!/bin/sh

export AG_APPLICATIONS=$TESTDIR
export AG_SERVICES=$TESTDIR
export AG_SERVICE_TYPES=$TESTDIR
export AG_PROVIDERS=$TESTDIR
export ACCOUNTS=/tmp/ 

$WRAPPER ./accountstest "$@"
