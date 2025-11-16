#!/bin/sh
ODIN_ts=$1;shift

if [ "$ODINVERBOSE" != "" ] ; then
   echo ${ODINRBSHOST}scan for references `basename $ODIN_ts`; fi

(
  ts_dir=$(dirname $ODIN_ts) &&
  cat $ODIN_ts |
  sed -n -e 's|^\s*///\s*<reference\s\s*path\s*=\s*"\([^"]*\).*$|\1|p' |
  while read name
  do
    case $name in
      /* )
         echo "'$name'"
         echo "=''"
         ;;
      * )
         echo "'$ts_dir/$name'"
         echo "=''"
         ;;
    esac
  done
) > ts-deps.view_desc 2>WARNINGS
if [ $? != 0 ]
then
  mv WARNINGS ERRORS
fi
exit 0
