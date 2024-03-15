#!/bin/sh

ODIN_tags_merge="$1" && shift &&
ODIN_sources=$(cat "$1") && shift &&
ODIN_outputType="$1" && shift &&

if [ "$ODINVERBOSE" != "" ] ; then
   set -x
fi &&

(  PATH="$ODIN_HCP_TAGS_PATH" \
       "$ODIN_tags_merge" $ODIN_sources 2>WARNINGS >$ODIN_outputType ||
	( mv WARNINGS ERRORS && echo "$hcptagsmerge failed" >> ERRORS )
   ) 2>&1
