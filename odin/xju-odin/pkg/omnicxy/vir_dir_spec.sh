#!/bin/sh

d="$1" && shift &&
(cd "$d" && ls -1)|
grep -E '\.hh$|\.cc$|\.hcp$'|
while read n
do
  echo "%$n==$d/$n"
done > omnicxy.vir_dir_spec.s
