#!/bin/sh -ex

PLUGIN_LS="$1"
PLUGIN_MODS_FILE="$1"
(
  echo "[mypy]"
  for p in $ODIN_MYPY_PLUGINS \
           $(test ! -r "$PLUGIN_LS" || cat "$PLUGIN_LS") \
           $(test ! -r "$PLUGIN_MODS_FILE" || cat "$PLUGIN_MODS_FILE")
  do
    echo $p
  done |
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
