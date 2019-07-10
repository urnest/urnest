#!/bin/sh

set -x &&
cd $(dirname "$0") &&
pwd &&
d=$(pwd) &&
export ODIN="$d/scratch" &&
export PATH="$d/scratch/bin:$PATH" &&
which odin &&

"$d/clean-install.sh" &&

$d/test-1.sh &&
$d/test-2.sh &&
$d/test-3.sh &&
$d/test-5.sh &&

echo PASS
