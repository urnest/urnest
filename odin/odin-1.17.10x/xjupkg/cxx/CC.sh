#! /bin/sh

ODIN_source=$1;shift;
ODIN_incsp=$1;shift;
ODIN_debug=$1;shift; ODIN_optimize=$1;shift;
ODIN_define=$1;shift; ODIN_cxx=$1;shift; ODIN_flags=$1;shift;
ODIN_abort=$1;shift;

if [ -n "$ODIN_cxx" ]
then
  x=`PATH="$ODIN_CXX_PATH" /usr/bin/which "$ODIN_cxx" 2>&1`
  if [ $? != 0 ] ; then
    ( echo "Error: Failed to locate executable named by +cxx option '$ODIN_cc' on path specified by ODIN_CXX_PATH (specified when Odin cache was created as'$ODIN_CXX_PATH') because"
      echo "$x" ) >&2
    exit 1
  fi
  compiler="$ODIN_cxx"
else
  x=`PATH="$ODIN_CXX_PATH" /usr/bin/which "$ODIN_CXX" 2>&1`
  if [ $? != 0 ] ; then
    ( echo "Error: Failed to locate executable named by ODIN_CXX on path specified by ODIN_CXX_PATH (specified when Odin cache was created as '$ODIN_CXX' and '$ODIN_CXX_PATH' respectively) because"
      echo "$x" ) >&2
    exit 1
  fi
  compiler="$ODIN_CXX"
fi

flags=""
if [ "$ODIN_debug" != "" ] ; then flags="$flags $ODIN_CXX_DEBUGF"; fi
if [ "$ODIN_optimize" != "" ] ; then flags="$flags -O$ODIN_optimize"; fi
if [ "$ODIN_define" != "" ] ; then
   for def in `cat $ODIN_define`; do
      flags="$flags -D$def"; done; fi
if [ "$ODIN_incsp" != "" ] ; then
   for sp in `cat $ODIN_incsp`; do
      flags="$flags -I$sp"; done; fi
if [ "$ODIN_CXX_I" != "" ] ; then
   for sp in $ODIN_CXX_I ; do
      flags="$flags -I$sp"; done; fi
if [ "$ODIN_flags" != "" ] ; then flags="$flags `cat $ODIN_flags`"; fi
flags="$flags $ODIN_CXX_FLAGS"

if [ "$ODINVERBOSE" != "" ] ; then
   echo ${ODINRBSHOST}$compiler -c $flags $ODIN_source; fi

PATH="$ODIN_CXX_PATH" \
LD_LIBRARY_PATH="$ODIN_CXX_LD_LIBRARY_PATH" \
$compiler -c $flags $ODIN_source \
  >MESSAGES 2>WARNINGS || {
    cat MESSAGES WARNINGS >ERRORS; rm MESSAGES WARNINGS;
    if [ ! -s ERRORS ] ; then 
      echo "$compiler failed" >>ERRORS; fi;
    if [ "$ODIN_CXX_IGNORE_ERR" != "" ] ; then
      if egrep -s -e "$ODIN_CXX_IGNORE_ERR" ERRORS; then
	 mv ERRORS WARNINGS; fi; fi; }
if [ -f MESSAGES ] ; then cat MESSAGES; rm MESSAGES; fi

input=`expr $ODIN_source : '.*/\([^/]*\)[.][^./]*'`
if [ -f $input.o ] ; then mv $input.o o; fi
if [ -f $input.obj ] ; then mv $input.obj o; fi

exit 0
