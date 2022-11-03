#!/bin/sh
set -e

ODIN_FILE=$1;shift; 
ODIN_env=$1;shift;
pySp="$1";shift;
if [ -z "$pySp" ]
then
    pySp=/dev/null
fi
PYPATH=$(
    cat "$pySp"|
	while read n
	do
	    echo -n ":$n"
	done
      )

L=""
if [ "$ODINVERBOSE" != "" ] ; then
   echo ${ODINRBSHOST}env - \`cat "$ODIN_env"\` LD_LIBRARY_PATH="$ODIN_MYPY_LD_LIBRARY_PATH" PATH="$ODIN_MYPY_PATH" PYTHONPATH="$ODIN_PYTHONPATH" MYPYPATH="$PYPATH:$ODIN_MYPYPATH" $ODIN_MYPY $ODIN_MYPY_FLAGS "$ODIN_FILE"; 
 fi
(
  eval env - `cat "$ODIN_env"` LD_LIBRARY_PATH="$ODIN_MYPY_LD_LIBRARY_PATH" PATH="$ODIN_MYPY_PATH" PYTHONPATH="$ODIN_PYTHONPATH" MYPYPATH="$PYPATH:$ODIN_MYPYPATH" "$ODIN_MYPY" $ODIN_MYPY_FLAGS $ODIN_FILE &&
  cat $ODIN_FILE > main.mypy3
) </dev/null 2>WARNINGS 1>&2 ||
mv WARNINGS ERRORS

exit 0
