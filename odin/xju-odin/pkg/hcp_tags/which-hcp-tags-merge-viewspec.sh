#!/bin/sh

option="$1" && shift &&
envvar="$ODIN_HCP_TAGS_PATH" &&
(
  if [ -n "$option" ]
  then
      echo "$option"
  fi &&
  echo "$envvar" | awk -F : '{ 
    for(i=1; i<=NF; ++i) {
      if ($i!="") print $i "/$ODIN_HCP_TAGS_MERGE";
    }
  }' &&
  echo "= '$ODIN_HCP_TAGS_MERGE'"
)  > which-hcp-tags-merge-viewspec  2>WARNINGS ||
mv WARNINGS ERRORS
