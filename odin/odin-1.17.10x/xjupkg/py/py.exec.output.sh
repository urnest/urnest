#!/bin/sh
V="$ODINVERBOSE" &&


output="$1" &&
status=$(cat "$2") &&
name="$3" &&

if [ $status = 0 ]
then
  echo "$output" > py.exec.output.ref
else
  ( echo "$(cat $name) failed with status $status and output:" &&
    cat "$output" ) > ERRORS 
fi
