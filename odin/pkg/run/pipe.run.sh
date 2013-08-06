#!/bin/sh

ODIN_FILE=$1;shift; ODIN_cmd=$1;shift;

cmd=cat
if [ "$ODIN_cmd" != "" ] ; then
   cmd=`cat $ODIN_cmd`; fi

if [ "$ODINVERBOSE" != "" ] ; then
   echo ${ODINRBSHOST}$cmd; fi

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

(mkdir output; cd output; LD_LIBRARY_PATH="$L" eval $cmd) \
 <$ODIN_FILE >stdout 2>WARNINGS ||
 { mv WARNINGS ERRORS; echo $cmd failed. >>ERRORS; }

exit 0
