#! /bin/sh

idl="$1" && shift &&
d="$1" && shift &&
hhext="$1" && shift &&
ccext="$1" && shift &&
basename_="$1" && shift &&
hpath="$1" && shift &&

if [ -z "$hhext" ]
then
  hhext="hh"
fi &&
if [ -z "$ccext" ]
then
  ccext="cc"
fi &&
if [ -n "$hpath" ]
then
  hpath="+hpath='$hpath'"
fi &&
b=$(basename "$idl" .idl) &&
if [ -n "$basename_" ]
then
  b="$basename_"
fi &&
bn="+basename='$b'" &&
bncref="+basename='$b.cref'" &&
bnobjref="+basename='$b.objref'" &&
bncdr="+basename='$b.cdr'" &&
bnsref="+basename='$b.sref'" &&
bnany="+basename='$b.any'" &&
( (
  cat > omnicxy.virdir-spec <<EOF
%$b.$hhext==($d/$b.hcp+hh-ext='$hhext'+cc-ext='$ccext'${bn}${hpath}:hcp.hh)
%$b.$ccext==($d/$b.hcp+hh-ext='$hhext'+cc-ext='$ccext'${bn}${hpath}:hcp.cc)
%$b.cref.$hhext==($d/$b.cref.hcp+hh-ext='$hhext'+cc-ext='$ccext'${bncref}${hpath}:hcp.hh)
%$b.cref.$ccext==($d/$b.cref.hcp+hh-ext='$hhext'+cc-ext='$ccext'${bncref}${hpath}:hcp.cc)
%$b.objref.$hhext==($d/$b.objref.hcp+hh-ext='$hhext'+cc-ext='$ccext'${bnobjref}${hpath}:hcp.hh)
%$b.objref.$ccext==($d/$b.objref.hcp+hh-ext='$hhext'+cc-ext='$ccext'${bnobjref}${hpath}:hcp.cc)
%$b.cdr.$hhext==($d/$b.cdr.hcp+hh-ext='$hhext'+cc-ext='$ccext'${bncdr}${hpath}:hcp.hh)
%$b.cdr.$ccext==($d/$b.cdr.hcp+hh-ext='$hhext'+cc-ext='$ccext'${bncdr}${hpath}:hcp.cc)
%$b.sref.$hhext==($d/$b.sref.hcp+hh-ext='$hhext'+cc-ext='$ccext'${bnsref}${hpath}:hcp.hh)
%$b.sref.$ccext==($d/$b.sref.hcp+hh-ext='$hhext'+cc-ext='$ccext'${bnsref}${hpath}:hcp.cc)
%$b.any.$hhext==($d/$b.any.hcp+hh-ext='$hhext'+cc-ext='$ccext'${bnany}${hpath}:hcp.hh)
%$b.any.$ccext==($d/$b.any.hcp+hh-ext='$hhext'+cc-ext='$ccext'${bnany}${hpath}:hcp.cc)
EOF
) 2>> WARNINGS || mv WARNINGS ERRORS )
