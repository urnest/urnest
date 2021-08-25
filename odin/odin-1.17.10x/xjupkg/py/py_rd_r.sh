#!/bin/sh

set -e

ODIN_file="$1"
ODIN_dirOfFile="$2"

echo "$ODIN_file+py_r=($ODIN_dirOfFile)" > py_rd_r
