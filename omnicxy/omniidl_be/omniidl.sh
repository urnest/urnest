#!/bin/sh

# pass full path name to backend .py file as first arg, rest
# of args are passed on to omniidl verbatim
#
be="$1" && shift &&
bedir=$(dirname "$be") &&
bename=$(basename "$be" .py) &&

/usr/local/omniORB-4.2.0/bin/omniidl -p "$bedir" -b "$bename" "$@"
