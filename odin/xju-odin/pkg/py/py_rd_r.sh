#!/bin/sh

set -e

ODIN_file="$1"
ODIN_py_r="$2"
ODIN_dirOfFile="$3"

if [ -n "$ODIN_py_r" ]
then
  echo "$ODIN_file+py_r=($ODIN_py_r)"
else
  echo "$ODIN_file+py_r=($ODIN_dirOfFile)"
fi  > py_rd_r
