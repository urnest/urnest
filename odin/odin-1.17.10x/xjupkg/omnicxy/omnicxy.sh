#! /bin/sh

source="$1" && shift &&
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

hhext="$1" && shift &&

pflags=$(for x in $bedirs ; do echo "-p$x"; done) &&
beflags=$(for x in $beopts ; do echo "-Wb$x"; done) &&
iflags=$(for x in $incsp ; do echo "-I$x"; done) &&
flags="$pflags $iflags" &&
if [ -n "$hpath" ]
then
  beflags="$beflags -Wb-hpath=$hpath"
fi &&
if [ -n "$hhext" ]
then
  beflags="$beflags -Wb-hhext=$hhext"
fi &&
(
PATH=$ODIN_OMNICXY_PATH &&
{ x=`/usr/bin/which "$ODIN_OMNICXY_OMNIIDL" 2>&1` 
if [ $? != 0 ] ; then
    ( echo "Error: Failed to locate executable named by ODIN_OMNICXY_OMNIIDL on path specified by ODIN_OMNICXY_PATH (specified when Odin cache was created as '$ODIN_OMNICXY_OMNIIDL' and '$ODIN_OMNICXY_PATH' respectively) because"
      echo "$x" ) >&2
    exit 1
fi ; } &&

mkdir omnicxy && 
( (
  cd omnicxy &&
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
  verbose "$omniidl $flags -b cxyany $beflags $source" &&
  "$omniidl" $flags -b cxyany $beflags "$source" > "$b.any.hcp" &&
  
  true
) 2>> WARNINGS || mv WARNINGS ERRORS )
if [ -r WARNINGS ] && [ -n "$ODIN_OMNICXY_SUPPRESS_WARNINGS" ]
then
    cat WARNINGS | egrep -v -e "$ODIN_OMNICXY_SUPPRESS_WARNINGS" |
	cat > WARNINGS.new &&
    mv WARNINGS.new WARNINGS
fi
)
