#!/bin/env sh
ODIN_dir=$1; shift

if [ "$ODINVERBOSE" != "" ] ; 
then
   echo ${ODINRBSHOST}"ls -a -1 \"$ODIN_dir\" | grep -E '[.]jar$|[.]JAR$'"
fi

ls -a -1 "$ODIN_dir"/* | grep -E '[.]jar$|[.]JAR$' > wildcard.jarlist.sm
exit 0
