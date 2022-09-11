#!/bin/sh

set -e

ODIN_file="$1"
ODIN_py_r="$2"
ODIN_py_main="$3"
ODIN_dirOfFile="$4"
ODIN_labelOfFile=$(cat "$5")

if [ -n "$ODIN_py_r" ]
then
  r=$ODIN_py_r
else
  r=$ODIN_dirOfFile
fi

if [ -n "$ODIN_py_main" ]
then
  m=$ODIN_py_main
else
  m=$ODIN_labelOfFile
fi

echo "$ODIN_file+py_r=($r)+py_main=$m"  > py_rd_r
