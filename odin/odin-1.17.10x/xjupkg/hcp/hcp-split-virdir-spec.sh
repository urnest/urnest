#! /bin/sh

source="$1" && shift &&
hhext="$1" && shift &&
ccext="$1" && shift &&
hh="$1" && shift &&
cc="$1" && shift &&

if [ -z "$hhext" ]
then
  hhext="hh"
fi &&
if [ -z "$ccext" ]
then
  ccext="cc"
fi &&
( (
  b=$(basename "$source" .hcp) &&
  cat > hcp-split-virdir-spec <<EOF
%$b.$hhext==$hh
%$b.$ccext==$cc
EOF
) 2>> WARNINGS || mv WARNINGS ERRORS )
