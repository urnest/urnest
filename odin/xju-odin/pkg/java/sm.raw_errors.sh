#!/bin/env sh

d=`dirname $0`
ODIN_sources="$1"; shift;
ODIN_target="$1"; shift;

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

if [ "$ODINVERBOSE" != "" ] ; 
then
   echo "${ODINRBSHOST} (isolated) javac `/bin/cat $ODIN_sources`"; 
fi

#need to isolate the sources from other sources to get all the
#errors
cat -n "$ODIN_sources"|
while read n s
do
  cp "$s" "$n.java"
  echo "$n.java"
done >> sources
mkdir output
(
  export PATH="$ODIN_JAVA_PATH"
  #set -x
  javac -Xmaxerrs 1000000 -implicit:none -d output -classpath . `/bin/cat sources` > \
    "$ODIN_target" 2>&1
)
rm -rf output
exit 0
