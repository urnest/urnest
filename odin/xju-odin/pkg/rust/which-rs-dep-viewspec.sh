#!/bin/sh

option="$1" && shift &&
envvar="$ODIN_RS_DEP_PATH" &&
(
  if [ -n "$option" ]
  then
      echo "'$option'"
  fi &&
  echo "$envvar" | awk -F : '{ 
    for(i=1; i<=NF; ++i) {
      if ($i!="") print "'" $i "/$ODIN_RS_DEP'";
    }
  }' &&
  echo "= 'rs-dep'"
)  > which-rs-dep-viewspec  2>WARNINGS ||
mv WARNINGS ERRORS
