#!/bin/sh -e

d=$(cd $(dirname "$0") && pwd)

rs=$1; shift

L=""
if [ -n "$1" ]
then
  L=$(cat "$1" | while read n ; do echo "-L $n" ; done)
fi; shift

cmd="$ODIN_RUSTC"

if [ -n "$1" ]; then cmd="$cmd -g"; fi; shift # +debug
if [ -n "$1" ]; then cmd="$cmd -O"; fi; shift # +optimize
if [ -n "$1" ]; then cmd="$cmd $(cat $1)"; fi; shift # +rs_flags

cmd="$cmd --crate-type bin"

verbose(){
  test -z "$ODINVERBOSE"||echo "$@"
}
verbose "$cmd $rs"
(
export PATH="$ODIN_RUSTC_PATH"
export LD_LIBRARY_PATH="$ODIN_RUSTC_LD_LIBRARY_PATH"
if ! which "$ODIN_RUSTC" >/dev/null
then
    echo "Error: rustc executable \"$ODIN_RUSTC\" (specified as ODIN_RUSTC when the Odin cache was created) is not executable via PATH \"$ODIN_RUSTC_PATH\" (specified when Odin cache was created as '$ODIN_RUSTC_PATH') - see $d/rust.dg" >&2
    false
fi
$cmd -o exe $rs 2>> WARNINGS || mv WARNINGS ERRORS
)
