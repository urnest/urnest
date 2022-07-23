#! /bin/sh

source="$1" && shift &&
hhext="$1" && shift &&
ccext="$1" && shift &&
basename_="$1" && shift &&
splitdir="$1" && shift &&

if [ -z "$hhext" ]
then
  hhext="hh"
fi &&
if [ -z "$ccext" ]
then
  ccext="cc"
fi &&
b=$(basename "$source" .hcp) &&
if [ -n "$basename_" ]
then
    b="$basename_"
fi &&
(
  echo "$splitdir/$b.$hhext" > hcp.hh.ref &&
  echo "$splitdir/$b.$ccext" > hcp.cc.ref &&
  echo "$splitdir/$b.$hhext.smap" > hcp.hh.smap.ref &&
  echo "$splitdir/$b.$ccext.smap" > hcp.cc.smap.ref &&
  (
  cat > hcp-split-virdir-spec <<EOF
%$b.$hhext==$splitdir/$b.$hhext
%$b.$ccext==$splitdir/$b.$ccext
%$b.$hhext.smap==$splitdir/$b.$hhext.smap
%$b.$ccext.smap==$splitdir/$b.$ccext.smap
EOF
) 2>> WARNINGS || mv WARNINGS ERRORS )
