#!/bin/sh

set -ex

ODIN_file="$1"
ODIN_py_r="$2"
ODIN_dirOfFile="$3"
ODIN_labelOfFile=$(cat "$4")

if [ -n "$ODIN_py_r" ]
then
  r=$ODIN_py_r
else
  r=$ODIN_dirOfFile
fi

echo "$ODIN_file+py_r=($r)+py_main=$ODIN_labelOfFile"  > py_rd_r
