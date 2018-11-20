#!/bin/sh
V="$ODINVERBOSE" &&


files="$1" &&
errors="$2" &&
status=$(cat "$3") &&
name="$4" &&
output="$5" &&
stderr="$6" &&

if [ $status = 0 ]
then
  echo "$files" > py.exec.files.ref
else
  ( echo "$(cat $name) failed with status $status and output:" &&
    if [ $stderr = "error" ]
    then
      cat "$errors"
    else
      cat "$output"
    fi
  ) > ERRORS 
fi
