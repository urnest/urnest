#!/bin/sh

ODIN_source=$1; shift;
ODIN_debug=$1; shift;
ODIN_classpath=$1; shift;

x=`PATH="$ODIN_JAVA_PATH" /usr/bin/which "jar" 2>&1`
if [ $? != 0 ] ; then
    ( echo "Error: Failed to locate executable jar on path specified by ODIN_JAVA_PATH (specified when Odin cache was created as '$ODIN_JAVA_PATH') because"
      echo "$x" ) >&2
    exit 1
fi

cat > manifest <<EOF
Manifest-Version: 1.0
Created-By: 1.6.0 (Sun Microsystems Inc.)
EOF

if [ "$ODINVERBOSE" != "" ] ; 
then
   echo "${ODINRBSHOST} jar -cf jar" 2>&1
fi

PATH="$ODIN_JAVA_PATH" jar -cmf manifest jar >MSGS 2>&1 || {
  mv MSGS ERRORS
  echo "\"PATH=\"$ODIN_JAVA_PATH\" jar -cf jar\" failed." >> ERRORS;
}
