#!/bin/sh

set -x &&
cd $(dirname "$0") &&
pwd &&
d=$(pwd) &&
which odin &&
"$d/clean-install.sh" &&

export ODIN="$d/scratch" &&
export PATH="$d/scratch/bin:$PATH" &&

$d/test-1.sh &&
$d/test-2.sh &&
$d/test-3.sh &&

echo PASS
