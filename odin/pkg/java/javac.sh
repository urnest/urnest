#!/bin/sh

ODIN_source=$1; shift;
ODIN_classnames=$1; shift;
ODIN_classpath=$1; shift;
ODIN_jar=$1; shift;
ODIN_debug=$1; shift;
ODIN_odir=$1; shift;

if [ -z "$ODIN_JAVA_OUTPUT_DIRECTORY" ]
then
    ( echo "Error: '$ODIN_JAVA_OUTPUT_DIRECTORY' is not set, it must be set when Odin cache is created, to locate the root output directory for java class files" ) >&2
    exit 1
fi
if [ ! -d "$ODIN_JAVA_OUTPUT_DIRECTORY" ]
then
    ( echo "Error: $ODIN_JAVA_OUTPUT_DIRECTORY (specified as '$ODIN_JAVA_OUTPUT_DIRECTORY' when Odin cache was created) is not a directory" ) >&2
    exit 1
fi
if [ ! -w "$ODIN_JAVA_OUTPUT_DIRECTORY" ]
then
    ( echo "Error: directory $ODIN_JAVA_OUTPUT_DIRECTORY (specified as '$ODIN_JAVA_OUTPUT_DIRECTORY' when Odin cache was created) is not writeable" ) >&2
    exit 1
fi

x=`PATH="$ODIN_JAVA_PATH" /usr/bin/which "javac" 2>&1`
if [ $? != 0 ] ; then
    ( echo "Error: Failed to locate executable javac on path specified by ODIN_JAVA_PATH (specified when Odin cache was created as '$ODIN_JAVA_PATH') because"
      echo "$x" ) >&2
    exit 1
fi

#REVISIT: classpath cygwin v windows
classpath="-cp $ODIN_JAVA_OUTPUT_DIRECTORY"
if [ -n "$ODIN_classpath" ]
then
  for n in `cat "$ODIN_classpath"`
  do
    classpath="$classpath:$n"
  done
fi
if [ -n "$ODIN_jar" ]
then
  for n in `cat "$ODIN_jar"`
  do
    if [ -d "$n" ]
    then
      classpath="$classpath:$n.*"
    else
      classpath="$classpath:$n"
    fi
  done
fi

debug=""
if [ -n "$ODIN_debug" ]
then
  debug="-g"
fi

mkdir output

if [ "$ODINVERBOSE" != "" ] ; 
then
   echo "${ODINRBSHOST} javac -d $ODIN_JAVA_OUTPUT_DIRECTORY $debug $classpath $ODIN_source" 2>&1
fi
source="$ODIN_source"
classname=`egrep "$source " "$ODIN_classnames" | awk '{print $2}'`
if [ "$classname" != "$source" ] && [ "$classname" != `basename "$source" .java` ]
then
  cp "$ODIN_source" "$classname.java"
  source="$classname.java"
fi
PATH="$ODIN_JAVA_PATH" javac -implicit:none -d output $debug $classpath "$source" >WARNINGS 2>&1 || {
  mv WARNINGS ERRORS;
  rm -r output;
  exit 0;
}

(
  set -x &&
  (cd output && tar cf ../class.tar .) &&
  cat class.tar | (cd $ODIN_JAVA_OUTPUT_DIRECTORY && tar x) &&
  #we keep just the content as the class value, since the tar
  #changes even when the content doesn't, because it keeps timestamps
  #REVISIT: no need to tar at all, can move the output files into
  #the output directory after catting them, but need to not move
  #directories where they already exist in the output area
  ( cd output && find . -type f -print | xargs cat ) >class &&
  rm -r output
) >LOG 2>&1

if [ $? != 0 ]
then
  echo "post compile failed:" >&2
  cat LOG >&2 &&
  rm LOG
  exit 1
fi
