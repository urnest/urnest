#!/bin/sh
V="$ODINVERBOSE" &&


output="$1" &&
error="$2" &&
status=$(cat "$3") &&
name="$4" &&
stderr="$5" &&

if [ $status = 0 ]
then
  echo "$output" > py.exec.output.ref
else
  ( echo "$(cat $name) failed with status $status and output:" &&
    if [ $stderr = "error" ]
    then
      cat "$error"
    else
      cat "$output"
    fi
  ) > ERRORS 
fi

