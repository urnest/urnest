#!/bin/sh

ODIN_FILE=$1;shift; 
ODIN_cmd=$1;shift;
ODIN_env=$1;shift;

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
  {
    (
      cd files &&
      eval env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" `cat "$ODIN_env"` $cmd 2>&1
    ) >output
    echo $? > status
  } &&
  if [ -z "`ls files`" ]
  then
    ( cd files && tar cf - . )
  else
    ( cd files && tar cf - * )
  fi > files.tar &&
  rm -rf files
) \
 <$ODIN_FILE 2>WARNINGS 1>&2 ||
mv WARNINGS ERRORS

exit 0
