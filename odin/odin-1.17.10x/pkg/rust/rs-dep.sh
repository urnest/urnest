#!/bin/sh -e

rsdep="$1"; shift

I=""
if [ -n "$1" ]
then
  I="-I $1"
fi; shift

rs=$1; shift

d=$(cd $(dirname "$0") && pwd)

verbose(){
  test -z "$ODINVERBOSE"||echo "$@"
}

(
export PATH="$ODIN_RS_DEP_PATH" &&
export LD_LIBRARY_PATH="$ODIN_RS_DEP_LD_LIBRARY_PATH" &&
if [ ! -x "$rsdep" ]
then
    echo "Error: rs-dep executable \"$rsdep\" is not executable. (By default this is name specified by ODIN_RS_DEP (specified when Odin cache was created as '$ODIN_RS_DEP') on path specified ODIN_RS_DEP_PATH (specified when Odin cache was created as '$ODIN_RS_DEP_PATH') but can be overridden by +rs_dep param - see $d/rust.dg" >&2
    false
fi &&


(
  verbose "$rsdep $I $rs" &&
  $rsdep -m mod-deps.view_desc -cs crate-src-deps.view_desc $I $rs
) 2>> WARNINGS || mv WARNINGS ERRORS

)
