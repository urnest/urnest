#!/bin/sh

idl="$1" && shift &&
d="$1" && shift &&

b=$(basename "$idl" .idl) &&

cat > omnicxy.hcp.list.ls <<EOF
$d/$b.hcp
$d/$b.cref.hcp
$d/$b.objref.hcp
$d/$b.cdr.hcp
$d/$b.sref.hcp
$d/$b.any.hcp
EOF
