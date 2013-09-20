#!/bin/sh

sources="$1"; shift
compilation="$1"; shift
target="$1"; shift

cat "$sources" |
while read n classname
do
  package="$(egrep '^[ \t]*package[ \t]+[A-Za-z0-9_.]*[ \t]*;' "$n" | sed 's=[ \t]*package[ \t][ \t]*\([A-Za-z0-9_.]*\).*=\1=g')"
  if [ -z "$package" ]
  then
      echo "no package statement found in file $n" >> ERRORS
  fi
  if [ $(echo "$package"|wc -w) -gt 1 ]
  then
      echo "multiple package statements found in file $n: $package" >> ERRORS
  fi
  if [ "$n" = "$classname" ]
  then
    class="$(basename "$n" .java)"
  else
    class="$classname"
  fi
  echo "%${package}.${class} == ${compilation}"
done > "$target"
