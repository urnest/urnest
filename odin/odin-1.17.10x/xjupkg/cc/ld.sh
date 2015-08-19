#!/bin/sh

ODIN_a=$1;shift; ODIN_lib=$1;shift; ODIN_home=$1;shift;
ODIN_gnu=$1;shift; ODIN_debug=$1;shift;
ODIN_prof=$1;shift; ODIN_cc=$1;shift; ODIN_flags=$1;shift;

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

if [ "$ODIN_CC_LD_LIBRARY_PATH" != "" ] ; then
   LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ODIN_CC_LD_LIBRARY_PATH;
   export LD_LIBRARY_PATH; fi


flags=""
if [ "$ODIN_debug" != "" ] ; then flags="$flags $ODIN_CC_DEBUGF"; fi
if [ "$ODIN_prof" != "" ] ; then flags="$flags -pg"; fi
if [ "$ODIN_flags" != "" ] ; then flags="$flags `cat $ODIN_flags`"; fi
flags="$flags $ODIN_CC_FLAGS"

libs=""
if [ "$ODIN_lib" != "" ] ; then libs=`cat $ODIN_lib`; fi

if [ "$ODINVERBOSE" != "" ] ; then
   echo ${ODINRBSHOST}$compiler $flags $ODIN_a $libs; fi

exe=`pwd`/exe
(
   PATH="$ODIN_CC_PATH" $compiler $flags $ODIN_a $libs $ODIN_CC_LD_EXTRA_LIBS -o $exe &&
   #
   # for windows, e.g. mingw
   #
   if [ -f $exe.exe ]
   then
      /bin/mv $exe.exe $exe
   fi
) \
>MESSAGES 2>WARNINGS || {
   cat MESSAGES WARNINGS >ERRORS; rm MESSAGES WARNINGS;
   if [ ! -s ERRORS ] ; then 
      echo "$compiler failed" >>ERRORS; fi;
   if [ "$ODIN_CC_IGNORE_ERR" != "" ] ; then
      if egrep -s -e "$ODIN_CC_IGNORE_ERR" ERRORS; then
	 mv ERRORS WARNINGS; fi; fi; }
if [ -f MESSAGES ] ; then cat MESSAGES; rm MESSAGES; fi

exit 0
