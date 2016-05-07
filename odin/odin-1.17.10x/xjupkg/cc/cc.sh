#!/bin/sh

ODIN_source=$1;shift; ODIN_incsp=$1;shift;
ODIN_home=$1;shift; ODIN_gnu=$1;shift; ODIN_debug=$1;shift;
ODIN_prof=$1;shift; ODIN_optimize=$1;shift; ODIN_define=$1;shift;
ODIN_cc=$1;shift; ODIN_flags=$1;shift;

if [ -n "$ODIN_cc" ]
then
  x=`PATH="$ODIN_CC_PATH" /usr/bin/which "$ODIN_cc" 2>&1`
  if [ $? != 0 ] ; then
    ( echo "Error: Failed to locate executable named by +cc option '$ODIN_cc' on path specified by ODIN_CC_PATH (specified when Odin cache was created as'$ODIN_CC_PATH') because"
      echo "$x" ) >&2
    exit 1
  fi
  compiler="$ODIN_cc"
else
  x=`PATH="$ODIN_CC_PATH" /usr/bin/which "$ODIN_CC" 2>&1`
  if [ $? != 0 ] ; then
    ( echo "Error: Failed to locate executable named by ODIN_CC on path specified by ODIN_CC_PATH (specified when Odin cache was created as '$ODIN_CC' and '$ODIN_CC_PATH' respectively) because"
      echo "$x" ) >&2
    exit 1
  fi
  compiler="$ODIN_CC"
fi

flags=""
if [ "$ODIN_debug" != "" ] ; then flags="$flags $ODIN_CC_DEBUGF"; fi
if [ "$ODIN_prof" != "" ] ; then flags="$flags -pg"; fi
if [ "$ODIN_optimize" != "" ] ; then flags="$flags -O$ODIN_optimize"; fi
if [ "$ODIN_define" != "" ] ; then
   for def in `cat $ODIN_define`; do
      flags="$flags -D$def"; done; fi
if [ "$ODIN_incsp" != "" ] ; then
   for sp in `cat $ODIN_incsp`; do
      flags="$flags -I$sp"; done; fi
if [ "$ODIN_flags" != "" ] ; then flags="$flags `cat $ODIN_flags`"; fi
flags="$flags $ODIN_CC_FLAGS"

if [ "$ODINVERBOSE" != "" ] ; then
   echo ${ODINRBSHOST}$compiler -c $flags $ODIN_source; fi

PATH="$ODIN_CC_PATH" \
LD_LIBRARY_PATH="$ODIN_CC_LD_LIBRARY_PATH" \
$compiler -c $flags $ODIN_source  >MESSAGES 2>WARNINGS || {
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
