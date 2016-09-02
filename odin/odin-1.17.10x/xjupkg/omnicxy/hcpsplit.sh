#! /bin/sh

sourcedir="$1" && shift &&
hcpsplit="$1" && shift &&
hpath="$1" && shift &&
ntc="$1" && shift &&
if [ -z "$hcpsplit" ]
then
  hcpsplit='hcp-split'
fi &&
hcpflags="" &&
if [ -n "$hpath" ]
then
  hcpflags="-hpath $hpath"
fi &&
if [ -n "$ntc" ]
then
  hcpflags="$hcpflags -ntc"
fi &&
(
PATH=$ODIN_OMNICXY_PATH &&
{ x=`/usr/bin/which "$hcpsplit" 2>&1` 
if [ $? != 0 ] ; then
    ( echo "Error: Failed to locate executable named by +hcp-split ($hcpsplit) on path specified by ODIN_OMNICXY_PATH (specified when Odin cache was created as '$ODIN_OMNICXY_PATH') because"
      echo "$x" ) >&2
    exit 1
fi ; } &&

mkdir hcpsplit.output && 
( (
  cd hcpsplit.output &&
  verbose(){
    test -z "$ODINVERBOSE"||echo "$@"
  }
  ls -1 "$sourcedir"/*.hcp |
  while [ $? = 0 ] && read n
  do
    b=$(basename "$n" .hcp) &&
    verbose "$hcpsplit $hcpflags $n $b.hh $b.cc" &&
    $hcpsplit $hcpflags $n $b.hh $b.cc &&
    cp $n .
  done &&
  true
) 2>> WARNINGS || mv WARNINGS ERRORS )
)
