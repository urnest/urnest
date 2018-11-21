#!/bin/sh

ODIN_FILE=$1;shift; 
ODIN_cmd=$1;shift;
ODIN_env=$1;shift;
ODIN_stderr=$1;shift;
if [ -z "$ODIN_stderr" ]
then
    ODIN_stderr="output"
fi
cmd=cat
if [ "$ODIN_cmd" != "" ] ; then
   cmd=`cat $ODIN_cmd`; fi

L=""
if [ "$ODINVERBOSE" != "" ] ; then
   echo ${ODINRBSHOST}env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" \`cat "$ODIN_env"\` $cmd; 
fi
(
  mkdir exec &&
  cd exec &&
  mkdir files &&
  touch errors &&
  touch output &&
  (
    cd files &&
    if [ $ODIN_stderr = "trace" ]
    then
      eval env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" `cat "$ODIN_env"` $cmd 2>&1 >../output
      echo $? > ../status
    elif [ $ODIN_stderr = "output" ]
    then
      eval env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" `cat "$ODIN_env"` $cmd >../output 2>&1
      echo $? > ../status
    elif [ $ODIN_stderr = "error" ]
    then
      eval env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" `cat "$ODIN_env"` $cmd >../output 2>../errors
      echo $? > ../status
    elif [ $ODIN_stderr = "warn" ]
    then
      eval env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" `cat "$ODIN_env"` $cmd >../output
      echo $? > ../status
    else
      echo "error: +stderr, \"$ODIN_stderr\" is not one of trace, output, error, warn.">&2 &&
      false
    fi
  ) &&
  if [ -z "`ls files`" ]
  then
    ( cd files && tar cf - . )
  else
    ( cd files && tar cf - * )
  fi > files.tar &&
  rm -rf files

) <$ODIN_FILE 2>WARNINGS ||
mv WARNINGS ERRORS

exit 0
