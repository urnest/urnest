#!/bin/sh

ODIN_pytest3conf=$1;shift;
ODIN_FILE=$1;shift; 
ODIN_env=$1;shift;
ODIN_stderr=$1;shift;
ODIN_dir=$1;shift;
pySp="$1";shift;
ODIN_extra_env=$1;shift;

if [ -z "$pySp" ]
then
    pySp=/dev/null
fi
if [ -z "$ODIN_stderr" ]
then
    ODIN_stderr="output"
fi
PYPATH=$(
    echo -n "$ODIN_DIR" &&
    cat "$pySp"|
	while read n
	do
	    echo -n ":$n"
	done
      )

pytest3=$ODIN_PYTEST3

if [ -z "$ODIN_pytest3conf" ] ; then
  ODIN_pytest3conf=$ODIN_PYTEST3_CONF;
fi

if [ -n "$ODIN_extra_env" ] ; then
  ODIN_extra_env=$(cat "$ODIN_extra_env" |
    while read n ; do echo -n " $n" ; done )
fi

L=""
if [ "$ODINVERBOSE" != "" ] ; then
   echo ${ODINRBSHOST}env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHON3PATH" \`cat "$ODIN_env"\` PYTEST_PLUGINS="$ODIN_PYTEST3_PLUGINS" $ODIN_extra_env $pytest3 -c $ODIN_pytest3conf "$ODIN_FILE"; 
fi
(
  mkdir pytest3.exec &&
  cd pytest3.exec &&
  mkdir files &&
  touch errors &&
  touch output &&
  (
    cd files &&
    if [ $ODIN_stderr = "trace" ]
    then
      eval env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHON3PATH" `cat "$ODIN_env"`  PYTEST_PLUGINS="$ODIN_PYTEST3_PLUGINS" $ODIN_extra_env $pytest3 -c $ODIN_pytest3conf "$ODIN_FILE" 2>&1 >../output
      echo $? > ../status
    elif  [ $ODIN_stderr = "output" ]
    then
      eval env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHON3PATH" `cat "$ODIN_env"` PYTEST_PLUGINS="$ODIN_PYTEST3_PLUGINS" $pytest3 -c $ODIN_pytest3conf "$ODIN_FILE" >../output 2>&1
      echo $? > ../status
    elif  [ $ODIN_stderr = "error" ]
    then
      eval env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHON3PATH" `cat "$ODIN_env"` PYTEST_PLUGINS="$ODIN_PYTEST3_PLUGINS" $pytest3 -c $ODIN_pytest3conf "$ODIN_FILE" >../output 2>../errors
      echo $? > ../status
    elif [ $ODIN_stderr = "warn" ]
    then
      eval env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHON3PATH" `cat "$ODIN_env"` PYTEST_PLUGINS="$ODIN_PYTEST3_PLUGINS" $pytest3 -c $ODIN_pytest3conf "$ODIN_FILE" >../output
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
