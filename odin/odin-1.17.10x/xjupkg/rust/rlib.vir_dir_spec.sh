#!/bin/sh -e

source="$1"; shift
rlib="$1"; shift

crate_name=$(basename "$source" .rs | sed -e 's=-=_=g') &&
(
  (
  cat > rlib.vir_dir_spec <<EOF
%lib${crate_name}.rlib==$rlib
EOF
  ) 2>> WARNINGS || mv WARNINGS ERRORS
)
