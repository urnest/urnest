#!/bin/sh
set -e

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
   echo ${ODINRBSHOST}env - LD_LIBRARY_PATH="$ODIN_MYPY_LD_LIBRARY_PATH" PATH="$ODIN_MYPY_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHONPATH" MYPYPATH="$ODIN_MYPYPATH" \`cat "$ODIN_env"\` $ODIN_MYPY $ODIN_MYPY_FLAGS "$ODIN_FILE"; 
 fi
(
  eval env - LD_LIBRARY_PATH="$ODIN_MYPY_LD_LIBRARY_PATH" PATH="$ODIN_MYPY_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHONPATH" MYPYPATH="$ODIN_MYPYPATH" `cat "$ODIN_env"` "$ODIN_MYPY" $ODIN_MYPY_FLAGS $ODIN_FILE &&
  cat $ODIN_FILE > mypy3
) </dev/null 2>WARNINGS 1>&2 ||
mv WARNINGS ERRORS

exit 0
