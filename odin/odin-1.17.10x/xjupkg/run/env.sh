#!/bin/sh
env_exe=$1 && shift &&
ODIN_env=$1 && shift &&

if [ -n "$ODINVERBOSE" ]
then
  set -x
fi

cat "$ODIN_env" | "$env_exe" > env

