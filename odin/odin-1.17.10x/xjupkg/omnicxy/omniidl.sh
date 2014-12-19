#! /bin/sh

source="$1" && shift &&
hcpsplit="$1" && shift &&
if [ -z "$1" ]
then
  bedirs=""
else
  bedirs=$(cat "$1") 
fi && shift &&
if [ -z "$1" ]
then
  beopts=""
else
  beopts=$(cat "$1") 
fi && shift &&

pflags=$(for x in $bedirs ; do echo "-p$x"; done) &&
beflags=$(for x in $beopts ; do echo "-Wb$x"; done) &&

if [ -z "$hcpsplit" ]
then
  hcpsplit='hcp-split'
fi &&

(
PATH=$ODIN_OMNICXY_PATH &&
{ x=`/usr/bin/which "$hcpsplit" 2>&1` 
if [ $? != 0 ] ; then
    ( echo "Error: Failed to locate executable named by +hcp-split ($hcpsplit) on path specified by ODIN_OMNICXY_PATH (specified when Odin cache was created as '$ODIN_OMNICXY_PATH') because"
      echo "$x" ) >&2
    exit 1
fi ; } &&
{ x=`/usr/bin/which "$ODIN_OMNICXY_OMNIIDL" 2>&1` 
if [ $? != 0 ] ; then
    ( echo "Error: Failed to locate executable named by ODIN_OMNICXY_OMNIIDL on path specified by ODIN_OMNICXY_PATH (specified when Odin cache was created as '$ODIN_OMNICXY_OMNIIDL' and '$ODIN_OMNICXY_PATH' respectively) because"
      echo "$x" ) >&2
    exit 1
fi ; } &&

mkdir omnicxy.output && 
( (
  cd omnicxy.output &&
  verbose(){
    test -z "$ODINVERBOSE"||echo "$@"
  }
  b=$(basename "$source" .idl) &&
  
  omniidl=$ODIN_OMNICXY_OMNIIDL &&
  verbose "$omniidl $pflags -b cxy $beflags $source" &&
  "$omniidl" $pflags -b cxy $beflags "$source" > "$b.hcp" &&
  verbose "$omniidl $pflags -b cxycdr $beflags $source" &&
  "$omniidl" $pflags -b cxycdr $beflags "$source" > "$b.cdr.hcp" &&
  verbose "$omniidl $pflags -b cxycref $beflags $source" &&
  "$omniidl" $pflags -b cxycref $beflags "$source" > "$b.cref.hcp" &&
  verbose "$omniidl $pflags -b cxyobjref $beflags $source" &&
  "$omniidl" $pflags -b cxyobjref $beflags "$source" > "$b.objref.hcp" &&
  verbose "$omniidl $pflags -b cxysref $beflags $source" &&
  "$omniidl" $pflags -b cxysref $beflags "$source" > "$b.sref.hcp" &&
  
  verbose "$hcpsplit -l 0 $b.hcp $b.hh $b.cc" &&
  "$hcpsplit" -l 0 "$b.hcp" "$b.hh" "$b.cc" &&
  verbose "$hcpsplit -l 0 $b.cdr.hcp $b.cdr.hh $b.cdr.cc" &&
  "$hcpsplit" -l 0 "$b.cdr.hcp" "$b.cdr.hh" "$b.cdr.cc" &&
  verbose "$hcpsplit -l 0 $b.cref.hcp $b.cref.hh $b.cref.cc" &&
  "$hcpsplit" -l 0 "$b.cref.hcp" "$b.cref.hh" "$b.cref.cc" &&
  verbose "$hcpsplit -l 0 $b.objref.hcp $b.objref.hh $b.objref.cc" &&
  "$hcpsplit" -l 0 "$b.objref.hcp" "$b.objref.hh" "$b.objref.cc" &&
  verbose "$hcpsplit -l 0 $b.sref.hcp $b.sref.hh $b.sref.cc" &&
  "$hcpsplit" -l 0 "$b.sref.hcp" "$b.sref.hh" "$b.sref.cc" &&

  true
) 2>> WARNINGS || mv WARNINGS ERRORS )
)
