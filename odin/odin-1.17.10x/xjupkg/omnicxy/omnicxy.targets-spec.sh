#! /bin/sh

idl="$1" && shift &&
d="$1" && shift &&
hhext="$1" && shift &&
ccext="$1" && shift &&

if [ -z "$hhext" ]
then
  hhext="hh"
fi &&
if [ -z "$ccext" ]
then
  ccext="cc"
fi &&
( (
  b=$(basename "$idl" .idl) &&
  cat > omnicxy.targets-spec <<EOF
$b.$hhext==($d/$b.hcp+hh-ext='$hhext'+cc-ext='$ccext':hcp.hh)
$b.$ccext==($d/$b.hcp+hh-ext='$hhext'+cc-ext='$ccext':hcp.cc)
$b.cref.$hhext==($d/$b.cref.hcp+hh-ext='$hhext'+cc-ext='$ccext':hcp.hh)
$b.cref.$ccext==($d/$b.cref.hcp+hh-ext='$hhext'+cc-ext='$ccext':hcp.cc)
$b.objref.$hhext==($d/$b.objref.hcp+hh-ext='$hhext'+cc-ext='$ccext':hcp.hh)
$b.objref.$ccext==($d/$b.objref.hcp+hh-ext='$hhext'+cc-ext='$ccext':hcp.cc)
$b.cdr.$hhext==($d/$b.cdr.hcp+hh-ext='$hhext'+cc-ext='$ccext':hcp.hh)
$b.cdr.$ccext==($d/$b.cdr.hcp+hh-ext='$hhext'+cc-ext='$ccext':hcp.cc)
$b.sref.$hhext==($d/$b.sref.hcp+hh-ext='$hhext'+cc-ext='$ccext':hcp.hh)
$b.sref.$ccext==($d/$b.sref.hcp+hh-ext='$hhext'+cc-ext='$ccext':hcp.cc)
$b.any.$hhext==($d/$b.any.hcp+hh-ext='$hhext'+cc-ext='$ccext':hcp.hh)
$b.any.$ccext==($d/$b.any.hcp+hh-ext='$hhext'+cc-ext='$ccext':hcp.cc)
EOF
) 2>> WARNINGS || mv WARNINGS ERRORS )
