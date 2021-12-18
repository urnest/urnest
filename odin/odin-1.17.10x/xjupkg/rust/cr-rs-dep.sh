#!/bin/sh -e

rsdep="$1"

L=""
if [ -n "$2" ]
then
  L=$(cat "$2" | while read n ; do echo "-L $n" ; done)
fi
rs=$3

d=$(cd $(dirname "$0") && pwd)

(
export PATH="$ODIN_RS_DEP_PATH" &&
export LD_LIBRARY_PATH="$ODIN_RS_DEP_LD_LIBRARY_PATH" &&
if [ ! -x "$rsdep" ]
then
    ( echo "Error: rs-dep executable \"$rsdep\" is not executable. (By default this is name specified by ODIN_RS_DEP (specified when Odin cache was created as '$ODIN_RS_DEP') on path specified ODIN_RS_DEP_PATH (specified when Odin cache was created as '$ODIN_RS_DEP_PATH') but can be overridden by +rs-dep param - see $(d)/rust.dg" ) >&2
    exit 1
fi &&


( (
  verbose(){
    test -z "$ODINVERBOSE"||echo "$@"
  }
  verbose "$rsdep -C $L $rs" &&
  $rsdep -C -m cr-mod-deps.view_desc -c cr-crate-deps.view_desc $L $rs
) 2>> WARNINGS || mv WARNINGS ERRORS )
)
