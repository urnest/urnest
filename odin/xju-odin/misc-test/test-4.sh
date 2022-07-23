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
  a=$(odin "d1%x.virdir_spec:vir_dir:filename>") &&
  b=$(odin "d1/Odinfile%x.virdir_spec:vir_dir:filename>") &&
  c=$(cd d1 && odin "Odinfile%x.virdir_spec:vir_dir:filename>") &&
  d=$(cd d1 && odin "%x.virdir_spec:vir_dir:filename>") &&
  test "$a" = "$b" &&
  test "$a" = "$c" &&
  test "$a" = "$d" &&
  true
) &&

echo "PASS"
