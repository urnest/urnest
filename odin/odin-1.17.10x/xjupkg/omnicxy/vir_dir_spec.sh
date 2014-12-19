#!/bin/sh

d="$1" && shift &&
(cd "$d" && ls -1)|
egrep '\.hh$|\.cc$'|
while read n
do
  echo "%$n==$d/$n"
done > omnicxy.vir_dir_spec.s
