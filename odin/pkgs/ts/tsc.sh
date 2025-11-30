#!/bin/sh
d=$(cd $(dirname "$0") && pwd)

ts=$1;shift
ts_root_dirs=$1;shift
ts_flags=$1;shift;

root_dir=$ODIN_TS_ROOT_DIR
if [ -n "$ts_root_dirs" ]
then
  root_dir=$(cat $ts_root_dirs)
fi
number_of_root_dirs=$(echo "$root_dir" | wc -w)
if [ $number_of_root_dirs = 0 ]
then
  echo "Error: +ts_root_dir must be specified (because \$ODIN_TS_ROOT_DIR was empty at odin cache creation time)" >ERRORS
  exit 0
fi  
if [ $number_of_root_dirs != 1 ]
then
  echo "Error: exactly one +ts_root_dir must be specified (not $incsp = $number_of_root_dirs)" >ERRORS
  exit 0
fi

cmd="$ODIN_TSC --outFile js --sourceRoot $root_dir --sourceMap $ODIN_TSC_FLAGS"
if [ -n "$ts_flags" ]; then cmd="$cmd $(cat $ts_flags)"; fi

verbose(){
  test -z "$ODINVERBOSE"||echo "$@"
}
verbose "$cmd $ts"
(
  export PATH="$ODIN_TSC_PATH"
  if ! which "$ODIN_TSC" >/dev/null
  then
    echo "Error: tsc executable \"$ODIN_TSC\" (specified as ODIN_TSC when the Odin cache was created) is not executable via path \"$ODIN_TSC_PATH\" (specified when Odin cache was created as '$ODIN_TSC_PATH') - see $d/ts.dg" >&2
    false
  fi &&
  # typescript is not called tsc (totally s*** compiler) for nothing:
  # - writes errors to stdout
  # - replaces absolute file name with relative filename in error messages
  # ... microstuffed... sigh
  if ! x=$($cmd $ts)
  then
    echo "$x" |
      while read e
      do
        rel=$(expr "$e" : '\([^(]*\)')
        abs=$(cd $(dirname "$rel") && pwd)/$(basename "$rel")
        echo "$e" | sed -e "s=$rel.\([0-9]*\),\([0-9]*\).=$abs:\1:\2=g" >&2
      done
    false
  fi
) 2>WARNINGS || mv WARNINGS ERRORS
