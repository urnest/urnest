#! /bin/sh

source="$1" && shift &&
hcpsplit="$1" && shift &&
hhext="$1" && shift &&
ccext="$1" && shift &&
hpath="$1" && shift &&

if [ -z "$hhext" ]
then
  hhext="hh"
fi &&
if [ -z "$ccext" ]
then
  ccext="cc"
fi &&
hcpflags="-ntc" &&
if [ -n "$hpath" ]
then
  hcpflags="$hcpflags -hpath $hpath"
fi &&
(
export PATH="$ODIN_HCP_PATH" &&
export LD_LIBRARY_PATH="$ODIN_HCP_LD_LIBRARY_PATH" &&
if [ ! -x "$hcpsplit" ]
then
    ( echo "Error: hcp-split executable \"$hcpsplit\" is not executable. (By default this is hcp-split on path specified ODIN_HCP_PATH (specified when Odin cache was created as '$ODIN_HCP_PATH') but can be overridden by +hcp-split param - see $(pwd)/hcp.dg" ) >&2
    exit 1
fi &&


( (
  verbose(){
    test -z "$ODINVERBOSE"||echo "$@"
  }
  b=$(basename "$source" .hcp) &&
  verbose "$hcpsplit $hcpflags $source $b.$hhext $b.$ccext" &&
  $hcpsplit $hcpflags $source $b.$hhext $b.$ccext &&
  mv $b.$hhext hcp.hh &&
  mv $b.$ccext hcp.cc
) 2>> WARNINGS || mv WARNINGS ERRORS )
)
