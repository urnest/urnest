#!/bin/env sh

d=`dirname $0`
ODIN_sources=$1; shift;
ODIN_raw_errors=$1; shift;
ODIN_inventory=$1; shift
ODIN_sp=$1; shift;
ODIN_target=$1; shift;
ODIN_classnames_target=$1; shift

x=`PATH="$ODIN_JAVA_PATH" /usr/bin/which "javac" 2>&1`
if [ $? != 0 ] ; then
    ( echo "Error: Failed to locate executable javac on path specified by ODIN_JAVA_PATH (specified when Odin cache was created as '$ODIN_JAVA_PATH') because"
      echo "$x" ) >&2
    exit 1
fi
x=`PATH="$ODIN_JAVA_PATH" /usr/bin/which "python3" 2>&1`
if [ $? != 0 ] ; then
    ( echo "Error: Failed to locate executable python3 on path specified by ODIN_JAVA_PATH (specified when Odin cache was created as '$ODIN_JAVA_PATH') because"
      echo "$x" ) >&2
    exit 1
fi

if [ "$ODINVERBOSE" != "" ] ; then
   sources=`cat $ODIN_sources`
   echo "${ODINRBSHOST} jdep.sh: scan $sources for dependencies"; fi

mkdir output
(
  export PATH="$ODIN_JAVA_PATH"
  #set -x
  #export JDEP_TRACE=1
  $d/jdep.py "$ODIN_sources" "$ODIN_raw_errors" "$ODIN_inventory" "$ODIN_sp" > "$ODIN_target" 2>WARNINGS
) || (
  mv WARNINGS ERRORS
)
rm -rf output
exit 0
