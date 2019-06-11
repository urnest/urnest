#!/bin/sh

#./CLEAN && rm -rf ODINBOOT && rm -rf ~/odin-dev-inst && mkdir ~/odin-dev-inst && ./INSTALL ~/odin-dev-inst
step=1 &&

set -x &&
cd $(dirname "$0") &&
pwd &&
which odin &&
step=$((step+1)) &&
(
cd d1 &&  
id1=$(odin 'Odinfile:targets_ptr:id>') &&
id2=$(odin '.:targets_ptr:id>') &&
test "$id1" = "$id2" &&
true
) &&

step=$((step+1))
(
id1=$(odin 'd1/Odinfile:targets_ptr:id>') &&
id2=$(odin 'd1:targets_ptr:id>') &&
test "$id1" = "$id2" &&
true
) &&

# since we have turned Odinfile into . want to be able to display Odinfile
# (which we'll do when asked to display its directory)
step=$((step+1))
(
set +x &&
o1=$(odin 'd1/Odinfile>') &&
echo "$o1" | (set -x && diff d1/Odinfile - ) &&
o2=$(odin 'd1>') &&
echo "$o2" | (set -x && diff d1/Odinfile - ) &&
cd d1 &&
o3=$(odin 'Odinfile>') &&
echo "$o3" | (set -x && diff Odinfile - ) &&
o4=$(odin '.>') &&
echo "$o4" | (set -x && diff Odinfile - ) &&
set -x &&
if shouldBeError=$(odin 'Odinfile-less-dir>' 2>&1)
then
  false
fi &&  
true
) &&

echo "PASS"
