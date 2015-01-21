#!/bin/sh
ODIN_env=$1;shift;

if [ -n "$ODINVERBOSE" ]
then
  set -x
fi

e=`/bin/echo -e "s/\001//g"` &&
tail --lines=+2 "$ODIN_env" | sed -e "$e" |
  sed -e 's/"/\"/g' |
  sed -e 's/\([^=]*=\)\(.*\)/\1"\2"/g' > env
