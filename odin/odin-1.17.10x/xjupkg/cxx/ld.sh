#!/bin/sh
#
ODIN_a=$1 && shift &&
ODIN_lib=$1 && shift &&
ODIN_define=$1;shift; ODIN_incsp=$1;shift;
ODIN_debug=$1;shift; ODIN_cxx=$1;shift; ODIN_flags=$1;shift;

if [ "$ODIN_CXX_LD_LIBRARY_PATH" != "" ] ; then
   LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ODIN_CXX_LD_LIBRARY_PATH;
   export LD_LIBRARY_PATH; fi

compiler=$ODIN_CXX
if [ "$ODIN_cxx" != "" ] ; then compiler=$ODIN_cxx; fi
flags=""
if [ "$ODIN_define" != "" ] ; then
   for def in `cat $ODIN_define`; do
      flags="$flags -D$def"; done; fi
if [ "$ODIN_incsp" != "" ] ; then
   for sp in `cat $ODIN_incsp`; do
      flags="$flags -I$sp"; done; fi
if [ "$ODIN_debug" != "" ] ; then flags="$flags $ODIN_CXX_LD_DEBUGF"; fi
if [ "$ODIN_flags" != "" ] ; then flags="$flags `cat $ODIN_flags`"; fi
flags="$flags $ODIN_CXX_FLAGS"

libs=""
if [ "$ODIN_lib" != "" ] ; then libs=`cat $ODIN_lib`; fi

if [ "$ODINVERBOSE" != "" ] ; then
   echo ${ODINRBSHOST}$compiler $flags $ODIN_a $libs  $ODIN_CXX_LD_EXTRA_LIBS; fi

x=`PATH="$ODIN_CXX_PATH" /usr/bin/which "$compiler" 2>&1`
if [ $? != 0 ] ; then
    ( echo "Error: Failed to locate executable named by ODIN_CXX on path specified by ODIN_CXX_PATH (specified when Odin cache was created as '$ODIN_CXX' and '$ODIN_CXX_PATH' respectively) because"
      echo "$x" ) >&2
    exit 1
fi
exe=`pwd`/exe
(
   PATH="$ODIN_CXX_PATH" $compiler $flags $ODIN_a $libs $ODIN_CXX_LD_EXTRA_LIBS -o $exe &&
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
   if [ "$ODIN_CXX_IGNORE_ERR" != "" ] ; then
      if egrep -s -e "$ODIN_CXX_IGNORE_ERR" ERRORS; then
	 mv ERRORS WARNINGS; fi; fi; }
if [ -f MESSAGES ] ; then cat MESSAGES; rm MESSAGES; fi

exit 0
