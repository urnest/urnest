#!/bin/sh

dirs="$1"; shift
names="$1"; shift
require="$1"; shift
cat "$names" |
while read name
do
  cat "$dirs" |
  while read dir
  do
    echo "'$dir/$name'"|sed 's=[$]=\\$=g'
  done
  if [ -z "$require" ]
  then
    echo "= ''"
  else
    echo "= '$name'"|sed 's=[$]=\\$=g'
  fi
done > search_desc 2>WARNINGS ||
mv WARNINGS ERRORS
