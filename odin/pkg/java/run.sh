#!/bin/sh
ODIN_stdin=$1; shift
ODIN_classpath=$1; shift
ODIN_runclasspath=$1; shift
ODIN_jar=$1; shift
ODIN_cmd=$1; shift
ODIN_driver="$1"; shift
ODIN_vmopts="$1"; shift
ODIN_symbols="$1"; shift

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

x=`PATH="$ODIN_JAVA_PATH" /usr/bin/which "java" 2>&1`
if [ $? != 0 ] ; then
    ( echo "Error: Failed to locate executable java on path specified by ODIN_JAVA_PATH (specified when Odin cache was created as '$ODIN_JAVA_PATH') because"
      echo "$x" ) >&2
    exit 1
fi

classpath="-cp $ODIN_JAVA_OUTPUT_DIRECTORY"
if [ -n "$ODIN_classpath" ]
then
  for n in `cat "$ODIN_classpath"`
  do
    classpath="$classpath:$n"
  done
fi
if [ -n "$ODIN_runclasspath" ]
then
  for n in `cat "$ODIN_runclasspath"`
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

cmd=
if [ "$ODIN_cmd" != "" ] ; then
   cmd=`cat $ODIN_cmd`; fi

vmopt=
if [ -n "$ODIN_vmopts" ]
then
  vmopts=`cat "$ODIN_vmopts"`
fi

symbols=
if [ -n "$ODIN_symbols" ]
then
  symbols=`head -1 "$ODIN_symbols"`
fi

if [ "$ODINVERBOSE" != "" ] ; 
then
   echo "${ODINRBSHOST}" $ODIN_driver "java" "$classpath" $vmopts $symbols $cmd 2>&1
fi

for i in $ODIN_LD_LIBRARY_PATH
do
   if [ -z "$L" ]
   then
      L="$i"
   else
      L="$L:$i"
   fi
done

mkdir output; 
(
  cd output; 
  LD_LIBRARY_PATH="$L" \
  PATH="$ODIN_JAVA_PATH" \
  $ODIN_driver java $classpath $vmopts $symbols $cmd <$ODIN_stdin 
) >java.stdout 2>WARNINGS ||
  {
    mv WARNINGS ERRORS; 
    cat java.stdout >> ERRORS; 
    echo java $classpath $cmd failed. >>ERRORS; 
  }

( cd output && tar cf - . ) > java.output
rm -rf output

exit 0
