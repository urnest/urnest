#!/bin/sh

#./CLEAN && rm -rf ODINBOOT && rm -rf ~/odin-dev-inst && mkdir ~/odin-dev-inst && ./INSTALL ~/odin-dev-inst
step=1 &&

set -x &&
cd $(dirname "$0") &&
pwd &&
d=$(pwd) &&
which odin &&
step=$((step+1)) &&
export ODINVERIFYLEVEL=0 &&
(
  odin 'v2%x/b/bb' &&
  odin 'v2%x/b/bb:filename>' &&
  f=$(odin "v2%x:filename>") &&
  cat "$f/b/bb" &&
  odin 'v1%eighty-files' &&
  sleep 1 &&
  odin 'v2%x!ls -l' &&
  odin 'v2%x/a/c/cc' &&
  cat "$f/a/c/cc" &&
  true
) &&

echo "PASS"
