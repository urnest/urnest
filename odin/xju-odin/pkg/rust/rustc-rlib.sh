#!/bin/sh -e

d=$(cd $(dirname "$0") && pwd)

rs=$1; shift

cmd="$ODIN_RUSTC --crate-type rlib"

cmd="$cmd -L $1"; shift  # local crates virdir

L=""
if [ -n "$1" ]
then
  L=$(cat "$1" | while read n ; do echo "-L $n" ; done)
fi; shift

cmd="$cmd $L"  # crate dirs

if [ -n "$1" ]; then cmd="$cmd -g"; fi; shift # +debug
if [ -n "$1" ]; then cmd="$cmd -O"; fi; shift # +optimize
if [ -n "$1" ]; then cmd="$cmd $(cat $1)"; fi; shift # +rs_flags
# unbelievably, even though a file might be called libxxx.rlib, if it was compiled from
# the kosher xxx/mod.rs location, it will be rejected as crate xxx?
if [ $(basename $rs) = "mod.rs" ]
then
  cmd="$cmd --crate-name $(basename $(dirname $rs))"
fi
if [ -n "$ODIN_RUSTC_FLAGS" ]; then cmd="$cmd $ODIN_RUSTC_FLAGS"; fi

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

$cmd -o rlib $rs 2>> WARNINGS || mv WARNINGS ERRORS
)
