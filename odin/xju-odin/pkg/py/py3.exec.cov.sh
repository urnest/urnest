#!/bin/sh
V="$ODINVERBOSE" &&


cov="$1" &&
errors="$2" &&
status=$(cat "$3") &&
name="$4" &&
output="$5" &&
stderr="$6" &&
if [ -z "$stderr" ]
then
  stderr=output
fi &&

if [ $status = 0 ]
then
  echo "$cov" > py3.exec.cov.ref &&
  cat "$errors" > WARNINGS
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
