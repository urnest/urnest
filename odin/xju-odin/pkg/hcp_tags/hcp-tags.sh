#!/bin/sh

ODIN_source="$1" && shift &&
ODIN_outputType="$1" && shift &&
hcptags="$1" && shift &&
    
if [ "$ODINVERBOSE" != "" ] ; then
   set -x
fi &&

(  PATH="$ODIN_HCP_TAGS_PATH" \
       "$hcptags" $ODIN_source 2>WARNINGS >$ODIN_outputType ||
	( mv WARNINGS ERRORS && echo "$hcptags failed" >> ERRORS )
   ) 2>&1
