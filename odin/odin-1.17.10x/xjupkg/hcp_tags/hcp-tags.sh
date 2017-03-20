#!/bin/sh

ODIN_source="$1" && shift &&
ODIN_outputType="$1" && shift &&
    
x=`PATH="$ODIN_HCP_TAGS_PATH" /usr/bin/which "$ODIN_HCP_TAGS" 2>&1`
if [ $? != 0 ] ; then
  ( echo "Error: Failed to locate executable named by ODIN_HCP_TAGS (specified when Odin cache was created as '$ODIN_HCP_TAGS') on path specified by ODIN_HCP_TAGS_PATH (specified when Odin cache was created as '$ODIN_HCP_TAGS_PATH') because"
    echo "$x" ) >&2 &&
  false
fi &&
hcptags="$ODIN_HCP_TAGS" &&

if [ "$ODINVERBOSE" != "" ] ; then
   set -x
fi &&

(  PATH="$ODIN_HCP_TAGS_PATH" \
       "$hcptags" $ODIN_source 2>WARNINGS >$ODIN_outputType ||
	( mv WARNINGS ERRORS && echo "$hcptags failed" >> ERRORS )
   ) 2>&1
