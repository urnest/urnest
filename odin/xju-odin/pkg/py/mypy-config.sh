#!/bin/sh -ex

PLUGIN_LS="$1";shift
PLUGIN_MODS_FILE="$1";shift
pySp="$1";shift;
ODIN_flags="$1";shift;
if [ -z "$pySp" ]
then
    pySp=/dev/null
fi
PYPATH=$(
  cat "$pySp"|
    while read n
    do
      echo -n ":$n"
    done
  )

flags="$ODIN_MYPY_FLAGS"
      
if [ "$ODIN_flags" != "" ] ; then flags="$flags `cat $ODIN_flags`"; fi
      
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
    echo "# mypy_path=$PYPATH"
    echo "# " $flags
) > mypy.config
