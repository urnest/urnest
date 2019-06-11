#!/bin/sh

set -x &&
cd $(dirname "$0") &&
pwd &&
d=$(pwd) &&
which odin &&
/bin/rm -rf scratch &&
mkdir -p scratch &&
export ODIN="$d/scratch" &&
export PATH="$d/scratch/bin:$PATH" &&
( cd .. && ./CLEAN && rm -rf ODINBOOT && ./INSTALL "$d/scratch" ) &&
odin -R </dev/null &&

$d/test-1.sh &&
$d/test-2.sh &&

echo PASS
