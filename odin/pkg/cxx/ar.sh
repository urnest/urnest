#!/bin/sh

objs=`ls $1/*`

x=`PATH="$ODIN_CXX_AR_PATH" /usr/bin/which "$ODIN_CXX_AR" 2>&1`
if [ $? != 0 ] ; then
    ( echo "Error: Failed to locate executable named by ODIN_CXX_AR on path specified by ODIN_CXX_AR_PATH (specified when Odin cache was created as '$ODIN_CXX_AR' and '$ODIN_CXX_AR_PATH' respectively) because"
      echo "$x" ) >&2
    exit 1
fi

if [ "$ODIN_CXX_LD_LIBRARY_PATH" != "" ] ; then
   LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ODIN_CXX_LD_LIBRARY_PATH;
   export LD_LIBRARY_PATH; fi

compiler="$ODIN_CXX_AR"

flags="$ODIN_CXX_AR_FLAGS"
if [ "$ODINVERBOSE" != "" ] ; 
then
     echo $compiler $flags a $objs
fi

PATH="$ODIN_CXX_AR_PATH" $compiler $flags a $objs >STDOUT 2>WARNINGS \
 || { mv WARNINGS ERRORS; echo 'ar failed' >>ERRORS; }

cat STDOUT
exit 0
