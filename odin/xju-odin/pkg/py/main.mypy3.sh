#!/bin/sh -e

ODIN_FILE=$1;shift; 
ODIN_CONFIG=$1;shift
ODIN_env=$1;shift;
pySp="$1";shift;
ODIN_flags="$1";shift;
ODIN_config_id=$(cat $1);shift;

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
      
cache_dir=$ODIN/$(/bin/hostname -f)/mypy.cache.$ODIN_config_id
mkdir -p "$cache_dir"

L=""
if [ "$ODINVERBOSE" != "" ] ; then
   echo ${ODINRBSHOST}env - \`cat "$ODIN_env"\` LD_LIBRARY_PATH="$ODIN_MYPY_LD_LIBRARY_PATH" PATH="$ODIN_MYPY_PATH" PYTHONPATH="$ODIN_MYPY_PYTHONPATH" MYPYPATH="$PYPATH:$ODIN_MYPY_MYPYPATH" "$ODIN_MYPY" --cache-dir "$cache_dir" --config-file $ODIN_CONFIG $flags "$ODIN_FILE"; 
 fi
(
  eval env - `cat "$ODIN_env"` LD_LIBRARY_PATH="$ODIN_MYPY_LD_LIBRARY_PATH" PATH="$ODIN_MYPY_PATH" PYTHONPATH="$ODIN_MYPY_PYTHONPATH" MYPYPATH="$PYPATH:$ODIN_MYPY_MYPYPATH" "$ODIN_MYPY" --cache-dir "$cache_dir" --config-file $ODIN_CONFIG $flags $ODIN_FILE &&
  cat $ODIN_FILE > main.mypy3
) </dev/null 2>WARNINGS 1>&2 ||
mv WARNINGS ERRORS

exit 0
