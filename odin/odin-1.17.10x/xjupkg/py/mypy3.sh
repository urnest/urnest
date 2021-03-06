#!/bin/sh

ODIN_FILE=$1;shift; 
ODIN_env=$1;shift;
ODIN_dir=$1;shift;
pySp="$1";shift;
if [ -z "$pySp" ]
then
    pySp=/dev/null
fi
PYPATH=$(
    echo -n "$ODIN_DIR" &&
    cat "$pySp"|
	while read n
	do
	    echo -n ":$n"
	done
      )

L=""
if [ "$ODINVERBOSE" != "" ] ; then
   echo ${ODINRBSHOST}env - LD_LIBRARY_PATH="$ODIN_MYPY_LD_LIBRARY_PATH" PATH="$ODIN_MYPY_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHON3PATH" \`cat "$ODIN_env"\` "$ODIN_MYPY" "$ODIN_FILE"; 
fi
(
  eval env - LD_LIBRARY_PATH="$ODIN_MYPY_LD_LIBRARY_PATH" PATH="$ODIN_MYPY_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHON3PATH" `cat "$ODIN_env"` "$ODIN_MYPY" "$ODIN_FILE" &&
  touch mypy3
) </dev/null 2>WARNINGS ||
mv WARNINGS ERRORS

exit 0
