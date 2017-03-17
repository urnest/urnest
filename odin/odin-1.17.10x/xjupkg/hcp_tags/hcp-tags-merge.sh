#!/bin/sh

ODIN_sources=$(cat $1) && shift &&

x=`PATH="$ODIN_HCP_TAGS_PATH" /usr/bin/which "$ODIN_HCP_TAGS_MERGE" 2>&1`
if [ $? != 0 ] ; then
  ( echo "Error: Failed to locate executable named by ODIN_HCP_TAGS_MERGE (specified when Odin cache was created as '$ODIN_HCP_TAGS_MERGE') on path specified by ODIN_HCP_TAGS_PATH (specified when Odin cache was created as '$ODIN_HCP_TAGS_PATH') because"
    echo "$x" ) >&2 &&
  false
fi &&
hcptagsmerge="$ODIN_HCP_TAGS_MERGE" &&

if [ "$ODINVERBOSE" != "" ] ; then
   set -x
fi &&

(  PATH="$ODIN_HCP_TAGS_PATH" \
       "$hcptagsmerge" $ODIN_sources 2>WARNINGS >tree_tags ||
	( mv WARNINGS ERRORS && echo "$hcptagsmerge failed" >> ERRORS )
   ) 2>&1
