#!/bin/sh

#./CLEAN && rm -rf ODINBOOT && rm -rf ~/odin-dev-inst && mkdir ~/odin-dev-inst && ./INSTALL ~/odin-dev-inst
step=1 &&

set -x &&
cd $(dirname "$0") &&
pwd &&
d=$(pwd) &&
which odin &&
odin -R < /dev/null &&
step=$((step+1)) &&
(
  /bin/rm -rf d2 &&
  mkdir d2 &&
  echo "X" > d2/X &&
  touch d2/Odinfile &&  
  odin 'd2:list' &&
  l=$(odin 'd2:list:ls>') &&
  test "$l" = "\
$d/d2/Odinfile
$d/d2/X" &&
  echo "Y==()+cmd=echo 'Y':stdout" > d2/Odinfile &&
  # REVISIT: not sure if we actually need the behaviour of the next two
  # tests - cannot think of a use case for them
  # if odin 'd2!:test'
  # then
  #  FAIL="directory should depend on contained Odinfile if it exists" && false
  #fi &&    
  #if odin 'd2/Odinfile!:test'
  #then
  #  false
  #fi &&
  l=$(odin 'd2:list:ls>') &&
  test "$l" = "\
$d/d2/Odinfile
$d/d2/X
$d/d2/Y" &&
  true
) &&

echo "PASS"
