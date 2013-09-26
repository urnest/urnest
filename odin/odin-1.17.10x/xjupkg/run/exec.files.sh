#!/bin/sh
V="$ODINVERBOSE" &&


files="$1" &&
status=$(cat "$2") &&
name="$3" &&
output="$4" &&

if [ $status = 0 ]
then
  echo "$files" > exec.files.ref
else
  ( echo "$(cat $name) failed with status $status and output:" &&
    cat "$output" ) > ERRORS 
fi
