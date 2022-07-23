#!/bin/sh
ODIN_exec2=$1; shift
ODIN_symbol=$1; shift
ODIN_stdin=$1; shift
ODIN_classpath=$1; shift
ODIN_runclasspath=$1; shift
ODIN_jar=$1; shift
ODIN_cmd=$1; shift
ODIN_driver="$1"; shift
ODIN_vmopts="$1"; shift
ODIN_default_stdin="$1"; shift
ODIN_target="$1"; shift

stdin="$ODIN_default_stdin"
if [ -n "$ODIN_stdin" ]
then
  stdin="$ODIN_stdin"
fi

if [ -n "$ODINVERBOSE" ]
then
echo "$ODIN_exec2" "$ODIN_symbol" "$ODIN_classpath" "$ODIN_runclasspath" \
  "$ODIN_jar" "$ODIN_cmd" "$ODIN_driver" "$ODIN_vmopts" \
  "<" "$stdin"
fi &&
mkdir -p "$ODIN_target"/files &&
(
  cd java.exec/files &&
  "$ODIN_exec2" "$ODIN_symbol" "$ODIN_classpath" "$ODIN_runclasspath" \
    "$ODIN_jar" "$ODIN_cmd" "$ODIN_driver" "$ODIN_vmopts" \
    < "$stdin" >../output 2>&1
  x=$?
  echo $x > ../status
  if [ $x != 0 ]
  then
    mv ../output ../errors
  fi
  if [ -n "`ls`" ]
  then
    ( cd files && tar cf ../files.tar * )
  fi;
) > ERRORS 2>&1 &&
mv ERRORS WARNINGS; 

rm -rf java.exec/files

exit 0
