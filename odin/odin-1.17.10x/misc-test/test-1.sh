#!/bin/sh

#./CLEAN && rm -rf ODINBOOT && rm -rf ~/odin-dev-inst && mkdir ~/odin-dev-inst && ./INSTALL ~/odin-dev-inst
step=1 &&

set -x &&
cd $(dirname "$0") &&
pwd &&
which odin &&
odin -R < /dev/null &&
step=$((step+1)) &&
(
id1=$(odin 'd1/Odinfile%x.virdir_spec:vir_dir:id>') &&
id2=$(odin 'd1%x.virdir_spec:vir_dir:id>') &&
test "$id1" = "$id2" &&
cd d1 &&  
id3=$(odin 'Odinfile%x.virdir_spec:vir_dir:id>') &&
id4=$(odin '.%x.virdir_spec:vir_dir:id>') &&
test "$id1" = "$id3" &&
test "$id1" = "$id4" &&
true
) &&

# want to be able to display Odinfile an we'll turn .> into that too
step=$((step+1)) &&
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
