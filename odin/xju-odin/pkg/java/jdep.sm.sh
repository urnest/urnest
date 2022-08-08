#!/bin/env sh

d=`dirname $0`
ODIN_sources=$1; shift;
ODIN_inventory=$1; shift
ODIN_sp=$1; shift;

x=`PATH="$ODIN_JAVA_PATH" /usr/bin/which "java" 2>&1`
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
   echo "${ODINRBSHOST} jdep.sh: scan " $(cat $ODIN_sources) " for dependencies"; fi

s=""
for ODIN_source in `cat $ODIN_sources`
do
  ln -s $ODIN_source
  s="$s `basename $ODIN_source`"
done

mkdir output
(
  export PATH="$ODIN_JAVA_PATH"
  #set -x
  #export JDEP_TRACE=1
  (javac -Xmaxerrs 1000000 -implicit:none -d output -classpath . $s 2>&1 || true) |
    $d/jdep.py "$ODIN_inventory" "$ODIN_sp" "$ODIN_debug" $ODIN_sources >java.view_desc 2>WARNINGS
  /bin/mv java.view_desc java.sm.view_desc
) || (
  mv WARNINGS ERRORS
)
rm -rf output
exit 0
