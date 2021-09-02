#!/bin/sh
env_cc=$1 && shift &&
ODIN_env=$1 && shift &&

if [ -n "$ODINVERBOSE" ]
then
  set -x
fi &&

env_exe=./env.exe &&

(
  export PATH=$ODIN_CXX_PATH &&
  $ODIN_CXX $ODIN_CXX_FLAGS $env_cc -o $env_exe
) &&

cat "$ODIN_env" | "$env_exe" > env

