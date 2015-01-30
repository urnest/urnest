#! /bin/sh

source="$1" && shift &&
hcpsplit="$1" && shift &&
hpath="$1" && shift &&
bedirs="$ODIN_OMNICXY_BE_DIR" &&
if [ -z "$1" ]
then
  bedirs="$ODIN_OMNICXY_BE_DIR"
else
  bedirs=$(echo $ODIN_OMNICXY_BE_DIR && cat "$1") 
fi && shift &&

if [ -z "$1" ]
then
  beopts=""
else
  beopts=$(cat "$1") 
fi && shift &&

if [ -n "$1" ]
then
  incsp=$(cat "$1")
fi && shift &&

pflags=$(for x in $bedirs ; do echo "-p$x"; done) &&
beflags=$(for x in $beopts ; do echo "-Wb$x"; done) &&
iflags=$(for x in $incsp ; do echo "-I$x"; done) &&
flags="$pflags $iflags" &&
if [ -z "$hcpsplit" ]
then
  hcpsplit='hcp-split'
fi &&
hcpflags="" &&
if [ -n "$hpath" ]
then
  hcpflags="-hpath $hpath"
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
  verbose "$omniidl $flags -b cxy $beflags $source" &&
  "$omniidl" $flags -b cxy $beflags "$source" > "$b.hcp" &&
  verbose "$omniidl $flags -b cxycdr $beflags $source" &&
  "$omniidl" $flags -b cxycdr $beflags "$source" > "$b.cdr.hcp" &&
  verbose "$omniidl $flags -b cxycref $beflags $source" &&
  "$omniidl" $flags -b cxycref $beflags "$source" > "$b.cref.hcp" &&
  verbose "$omniidl $flags -b cxyobjref $beflags $source" &&
  "$omniidl" $flags -b cxyobjref $beflags "$source" > "$b.objref.hcp" &&
  verbose "$omniidl $flags -b cxysref $beflags $source" &&
  "$omniidl" $flags -b cxysref $beflags "$source" > "$b.sref.hcp" &&
  
  verbose "$hcpsplit $hcpflags $b.hcp $b.hh $b.cc" &&
  "$hcpsplit" $hcpflags "$b.hcp" "$b.hh" "$b.cc" &&
  verbose "$hcpsplit $hcpflags $b.cdr.hcp $b.cdr.hh $b.cdr.cc" &&
  "$hcpsplit" $hcpflags "$b.cdr.hcp" "$b.cdr.hh" "$b.cdr.cc" &&
  verbose "$hcpsplit $hcpflags $b.cref.hcp $b.cref.hh $b.cref.cc" &&
  "$hcpsplit" $hcpflags "$b.cref.hcp" "$b.cref.hh" "$b.cref.cc" &&
  verbose "$hcpsplit $hcpflags $b.objref.hcp $b.objref.hh $b.objref.cc" &&
  "$hcpsplit" $hcpflags "$b.objref.hcp" "$b.objref.hh" "$b.objref.cc" &&
  verbose "$hcpsplit $hcpflags $b.sref.hcp $b.sref.hh $b.sref.cc" &&
  "$hcpsplit" $hcpflags "$b.sref.hcp" "$b.sref.hh" "$b.sref.cc" &&

  true
) 2>> WARNINGS || mv WARNINGS ERRORS )
)
