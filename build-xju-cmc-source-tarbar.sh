#!/bin/sh

set -e

d=$(cd $(dirname "$0") && pwd)

outd=$(pwd)

set -x

cd "$d"

version=$(git describe --tags --exact-match  |grep 'xju-cmc-' || true)

test $(echo $version | wc -w) -lt 2
if [ -z "$version" ]
then
  version="xju-cmc-unknown"
else
  if [ $(git status -s |wc -l) != 0 ]
  then
    version="xju-cmc-unknown"
  fi
fi  

(
  cd xju
  odin '(test-cmc.py)+(%py-opts):py3.exec.output'
  dependencies=$(odin 'cmc.py:py_rd+(%py-opts):py_import_all:ls>' | grep -E "$(pwd)/.*[.]py$" || true)
  if [ $(echo $dependencies | wc -w) != 0 ]
  then
    echo "ERROR: expected cmc.py to have no dependencies, not: $dependencies">&2
    false
  fi
)  
tar czf "$outd/$version.tar.gz" xju/cmc.py

echo "created $outd/$version.tar.gz"
