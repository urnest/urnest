#!/bin/sh
ODIN_tar_file="$1"; shift
ODIN_member="$1"; shift
ODIN_target_type="$1"; shift

w=`which which` || ( echo "which not found on path $PATH" >&2; false ) &&
t=`env - PATH="$ODIN_TAR_PATH" "$w" tar` &&

if [ -z "$ODIN_member" ]
then
  echo "+member must be specified and non-empty string">ERRORS
  exit 0
fi

if [ "$ODINVERBOSE" != "" ] ; 
then
   echo "${ODINRBSHOST} $t xf $ODIN_tar_file $ODIN_member"; 
fi &&

(
  mkdir files &&
  (
    cd files && env - PATH="$ODIN_TAR_PATH" "$t" xf "$ODIN_tar_file" "$ODIN_member" 
  ) &&
  mv "files/$ODIN_member" "$ODIN_target_type"
) 2>WARNINGS || mv WARNINGS ERRORS
rm -rf "files"
