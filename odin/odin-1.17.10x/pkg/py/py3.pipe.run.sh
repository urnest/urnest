#!/bin/sh

ODIN_STDIN=$1;shift;
ODIN_py=$1;shift;
ODIN_cmd=$1;shift;
ODIN_DIR=$1;shift
pySp=$1;shift
PYPATH=$(
    echo -n "$ODIN_DIR" &&
    cat "$pySp"|
	while read n
	do
	    echo -n ":$n"
	done
      )

cmd=cat
if [ "$ODIN_cmd" != "" ] ; then
   cmd=`cat $ODIN_cmd`; fi

if [ "$ODINVERBOSE" != "" ] ; then
   echo ${ODINRBSHOST}python3 $ODIN_py $cmd; fi

(mkdir output; cd output; LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHONPATH" eval python3 "$ODIN_py" $cmd) \
 <$ODIN_STDIN >py3.stdout 2>WARNINGS ||
 { mv WARNINGS ERRORS; echo $cmd failed. >>ERRORS; }

exit 0
