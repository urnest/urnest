#!/bin/sh

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
   echo ${ODINRBSHOST}env - LD_LIBRARY_PATH="$ODIN_PYSTYLECHECK_LD_LIBRARY_PATH" PATH="$ODIN_PYSTYLECHECK_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHONPATH" \`cat "$ODIN_env"\` $ODIN_PYSTYLECHECK "$ODIN_FILE"; 
fi
(
  eval env - LD_LIBRARY_PATH="$ODIN_PYSTYLECHECK_LD_LIBRARY_PATH" PATH="$ODIN_PYSTYLECHECK_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHONPATH" `cat "$ODIN_env"` "$ODIN_PYSTYLECHECK" $ODIN_FILE &&
  touch main.stylecheck
) </dev/null 2>WARNINGS 1>&2 ||
mv WARNINGS ERRORS

exit 0
