#!/bin/sh
V="$ODINVERBOSE" &&

if [ $# != 6 ]
then
  echo "assertion failure, $0 passed $# params, not 6"
  false
fi &&
cov="$1" &&
output="$2" &&
errors="$3" &&
status=$(cat "$4") &&
name="$5" &&
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
