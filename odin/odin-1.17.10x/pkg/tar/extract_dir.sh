#!/bin/sh
ODIN_tar_file="$1"; shift
ODIN_member="$1"; shift
ODIN_target_type="$1"; shift

if [ -z "$ODIN_member" ]
then
  files=`tar tf "$ODIN_tar_file" | egrep "^[^/][^/]*$"`
else
  files=`tar tf "$ODIN_tar_file" | egrep "^${ODIN_member}/[^/][^/]*$"` &&
  test -n "$files" || (
    echo "$ODIN_member does not match any directory in $1" ; exit 0
  )
fi

w=`which which`
t=`env - PATH="$ODIN_TAR_PATH" "$w" tar`
if [ "$ODINVERBOSE" != "" ] ; 
then
   echo "${ODINRBSHOST} $t xf $ODIN_tar_file $files"; 
fi

mkdir extract_dir &&
if [ -n "$files" ]
then
  mkdir out &&
  ( cd out && 
    { 
      PATH="$ODIN_TAR_PATH" \
      LD_LIBRARY_PATH="$ODIN_TAR_LD_LIBRARY_PATH" \
      tar xf "$ODIN_tar_file" $files >../WARNINGS 2>&1 || 
                mv ../WARNINGS ../ERRORS; } ) &&
  if [ ! -f ERRORS ]
  then
    ( cd out && mv -f $files ../extract_dir )
  fi
fi
x=$?
rm -rf out &&
exit $?
