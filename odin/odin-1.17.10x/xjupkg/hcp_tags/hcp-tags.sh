#!/bin/sh

ODIN_sources=$(cat $1) && shift &&

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
       "$hcptags" $ODIN_sources 2>WARNINGS >dir_tags ||
	( mv WARNINGS ERRORS && echo "$hcptags failed" >> ERRORS )
   ) 2>&1
