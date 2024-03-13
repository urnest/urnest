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
export ODINVERIFYLEVEL=0 &&
(
  odin 'd5%x.o' &&
  odin 'd5%x.o:depend:ls' &&
  odin "d5%x.o:depend:ls>" &&
  odin "d5%x.o:depend:ls>" | grep -F "$d/d5/x.cc" &&
  odin "d5%x.o:depend:ls>" | grep -F "$d/d5/hcp/tags/x.h" &&
  odin "d5%x.o:depend:ls>" | grep -F "$d/d5/Odinfile" &&
  odin "d5%x.o:depend:ls>" | grep -F "$d/d5/hcp/Odinfile" &&
  odin "d5%x.o:depend:ls>" | grep -F "$d/d5/hcp/tags/Odinfile" &&
  hcp=$(odin "d5/hcp%hcp.d:filename>") &&
  odin 'd5%x.o+depend=('$hcp')!:dpath' |grep -F -q 'is an input of' &&
  tags=$(odin "d5/hcp/tags%tags.d:filename>") &&
  odin 'd5%x.o+depend=('$tags')!:dpath' |grep -F -q 'is an input of' &&
  true
) &&

echo "PASS"
