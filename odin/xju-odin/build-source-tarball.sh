#!/bin/sh

set -e

d=$(cd $(dirname "$0") && pwd)

outd=$(pwd)

set -x

cd "$d"

./CLEAN

version=$(git describe --tags --exact-match  |grep 'xju-odin-' || true)

test $(echo $version | wc -c) -lt 2
if [ -z "$version" ]
then
  version="unknown"
else
  if [ $(git status -s |wc -l) != 0 ]
  then
    version="unknown"
  fi
fi  
cd ".."

tar czf "$outd/xju-odin-$version.tar.gz" xju-odin

echo "created $outd/xju-odin-$version.tar.gz"
