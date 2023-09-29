#!/bin/sh -ex

PLUGIN_LS="$1"
(
  echo "[mypy]"
  cat "$PLUGIN_LS" |
    if read n ;
    then
      echo -n "plugins=$n"
      while read n
      do
        echo -n ",$n"
      done
      echo ""
    fi
) > mypy.config
