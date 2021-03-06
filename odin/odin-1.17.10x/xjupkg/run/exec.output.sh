#!/bin/sh
V="$ODINVERBOSE" &&


output="$1" &&
errors="$2" &&
status=$(cat "$3") &&
name="$4" &&
stderr="$5" &&
if [ -z "$stderr" ]
then
  stderr=output
fi &&

if [ $status = 0 ]
then
  echo "$output" > exec.output.ref &&
  cat "$errors" > WARNINGS
else
  ( echo "$0:20:ERROR:$(cat $name) failed with status $status and output:" &&
    if [ $stderr = "error" ]
    then
      cat "$errors"
    else
      cat "$output"
    fi
  ) > ERRORS 
fi
