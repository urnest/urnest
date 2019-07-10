#!/bin/sh

set -x &&
cd $(dirname "$0") &&
pwd &&
d=$(pwd) &&
which odin &&
/bin/rm -rf scratch &&
mkdir -p scratch &&
( cd .. && ./CLEAN && rm -rf ODINBOOT && ./INSTALL "$d/scratch" ) &&
export ODIN="$d/scratch" &&
export PATH="$d/scratch/bin:$PATH" &&
odin -R </dev/null &&

true
