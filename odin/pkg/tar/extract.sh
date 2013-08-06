#!/bin/sh
ODIN_tar_file="$1"; shift
ODIN_member="$1"; shift
ODIN_target_type="$1"; shift

if [ -z "$ODIN_member" ]
then
  echo "+member must be specified and non-empty string">ERRORS
  exit 0
fi

w=`which which`
t=`env - PATH="$ODIN_TAR_PATH" "$w" tar`
if [ "$ODINVERBOSE" != "" ] ; 
then
   echo "${ODINRBSHOST} $t xf $ODIN_tar_file $ODIN_member"; 
fi

(
  mkdir files &&
  (
    cd files && env - PATH="$ODIN_TAR_PATH" "$t" xf "$ODIN_tar_file" "$ODIN_member" 
  ) &&
  mv "files/$ODIN_member" "$ODIN_target_type" &&
  rm -rf "files"
) 2>WARNINGS
if [ $? != 0 ]
then
  mv WARNINGS ERRORS
fi
