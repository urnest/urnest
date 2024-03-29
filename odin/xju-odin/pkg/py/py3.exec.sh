#!/bin/sh

ODIN_FILE=$1;shift &&
ODIN_cmd=$1;shift &&
ODIN_env=$1;shift &&
ODIN_stderr=$1;shift &&
ODIN_dir=$1;shift &&
pySp="$1";shift &&
nocov="$1";shift &&
runcov="$1";shift &&

if [ -z "$pySp" ]
then
  pySp=/dev/null
fi &&

if [ -z "$ODIN_stderr" ]
then
  ODIN_stderr="output"
fi &&

PYPATH=$(
  echo -n "$ODIN_DIR" &&
  cat "$pySp"|
    while read n
    do
      echo -n ":$n"
    done
  ) &&

if [ -z "$nocov" ]
then
  cov="$runcov ../run.py3.cov "
else
  cov=""
fi  &&

cmd= &&
if [ "$ODIN_cmd" != "" ]
then
  cmd=`cat $ODIN_cmd`
fi &&

L="" &&
if [ "$ODINVERBOSE" != "" ]
then
  echo ${ODINRBSHOST}env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" \`cat "$ODIN_env"\` PYTHONPATH="$PYPATH:$ODIN_PYTHONPATH" python3 "$ODIN_FILE" $cmd; 
fi &&
(
  mkdir py3.exec &&
  cd py3.exec &&
  env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHONPATH" python3 -c "import pickle; pickle.dump(({},{},{}),open('run.py3.cov','wb'))" &&
  mkdir files &&
  touch errors &&
  touch output &&
  touch WARNINGS &&  
  (
    cd files &&
    if [ $ODIN_stderr = "trace" ]
    then
      eval env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHONPATH" `cat "$ODIN_env"` python3 $cov "$ODIN_FILE" $cmd 2>&1 >../output
      echo $? > ../status
    elif  [ $ODIN_stderr = "output" ]
    then
      eval env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHONPATH" `cat "$ODIN_env"` python3 $cov "$ODIN_FILE" $cmd >../output 2>&1
      echo $? > ../status
    elif  [ $ODIN_stderr = "error" ]
    then
      eval env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHONPATH" `cat "$ODIN_env"` python3 $cov "$ODIN_FILE"  $cmd >../output 2>../errors
      echo $? > ../status
    elif [ $ODIN_stderr = "warn" ]
    then
      eval env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHONPATH" `cat "$ODIN_env"` python3 $cov "$ODIN_FILE" $cmd >../output 2>../../WARNINGS
      echo $? > ../status
    else
      echo "error: +stderr, \"$ODIN_stderr\" is not one of trace, output, error, warn.">&2
      false
    fi
  ) <$ODIN_FILE &&
  if [ -z "`ls files`" ]
  then
    ( cd files && tar cf - . )
  else
    ( cd files && tar cf - * )
  fi > files.tar &&
  rm -rf files
)
exit 0
