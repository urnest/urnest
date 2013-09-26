#!/bin/sh

ODIN_FILE=$1;shift; 
ODIN_cmd=$1;shift;
ODIN_env=$1;shift;

cmd=cat
if [ "$ODIN_cmd" != "" ] ; then
   cmd=`cat $ODIN_cmd`; fi

if [ "$ODINVERBOSE" != "" ] ; then
   echo ${ODINRBSHOST}env - LD_LIBRARY_PATH="$L" PATH="$ODIN_EXEC_PATH" \`cat "$ODIN_env"\` $cmd; 
fi

L=""
for i in $ODIN_LD_LIBRARY_PATH
do
   if [ -z "$L" ]
   then
      L="$i"
   else
      L="$L:$i"
   fi
done

(
  mkdir exec &&
  cd exec &&
  mkdir files &&
  {
    (
      cd files &&
      eval env - LD_LIBRARY_PATH="$L" PATH="$ODIN_EXEC_PATH" `cat "$ODIN_env"` $cmd 2>&1
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
