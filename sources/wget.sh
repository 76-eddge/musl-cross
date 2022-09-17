#!/bin/sh

wget "$@"
x=$?
[ $x -eq 1 ] && exit 0
exit $x
