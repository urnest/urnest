#!/bin/sh

#./CLEAN && rm -rf ODINBOOT && rm -rf ~/odin-dev-inst && mkdir ~/odin-dev-inst && ./INSTALL ~/odin-dev-inst
step=1 &&

set -x &&
cd $(dirname "$0") &&
pwd &&
d=$(pwd) &&
which odin &&
step=$((step+1)) &&
(
  /bin/rm -rf d2 &&
  mkdir d2 &&
  ( cat <<EOF
zzz==<<
content
EOF
) > d2/Odinfile &&
  odin "d2/zzz" &&
  zzz=$(cat d2/zzz) &&
  test "$zzz" = "content" &&
  ( cat <<EOF
zzz==<<
contents
EOF
) > d2/Odinfile &&
  ODIN_VERIFYLEVEL=0 odin "d2/Odinfile!:test" &&
  ODIN_VERIFYLEVEL=0 odin "d2/zzz" &&
  zzz=$(cat d2/zzz) &&
  test "$zzz" = "contents" &&
  true
) &&

echo "PASS"
