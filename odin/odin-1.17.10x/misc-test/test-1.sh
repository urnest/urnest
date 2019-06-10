#!/bin/sh

set -x &&
cd $(dirname "$0") &&
pwd &&
which odin &&
(
cd d1 &&  
id1=$(odin 'Odinfile:targets_ptr:id>') &&
id2=$(odin '.:targets_ptr:id>') &&
test "$id1" = "$id2" &&
true
) &&

(
id1=$(odin 'd1/Odinfile:targets_ptr:id>') &&
id2=$(odin 'd1:targets_ptr:id>') &&
test "$id1" = "$id2" &&
true
) &&

echo "PASS"

