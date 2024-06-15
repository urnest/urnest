#!/bin/sh

set -e

d=$(cd $(dirname "$0") && pwd)

outd=$(pwd)

set -x

cd "$d"

./CLEAN

version=$(git describe --tags --exact-match  |grep 'xju-odin-' || true)

test $(echo $version | wc -w) -lt 2
if [ -z "$version" ]
then
  version="xju-odin-unknown"
else
  if [ $(git status -s |wc -l) != 0 ]
  then
    version="xju-odin-unknown"
  fi
fi  

cd ".."

tar czf "$outd/$version.tar.gz" --exclude=.pytest_cache --exclude=__pycache__ --exclude '*~' xju-odin

echo "created $outd/$version.tar.gz"
