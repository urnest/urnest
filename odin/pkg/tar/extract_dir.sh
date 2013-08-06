#!/bin/sh
ODIN_tar_file="$1"; shift
ODIN_member="$1"; shift
ODIN_target_type="$1"; shift

if [ -z "$ODIN_member" ]
then
  echo "+member must be specified and non-empty string">ERRORS
  exit 0
fi
if [ -z "$ODIN_member" ]
then
  files=`tar tf "$ODIN_tar_file" | egrep "[^/][^/]*$"`
else
  files=`tar tf "$ODIN_tar_file" | egrep "${ODIN_member}/[^/][^/]*$"`
fi

w=`which which`
t=`env - PATH="$ODIN_TAR_PATH" "$w" tar`
if [ "$ODINVERBOSE" != "" ] ; 
then
   echo "${ODINRBSHOST} $t xf $ODIN_tar_file $files"; 
fi

(
  tar xf "$ODIN_tar_file" $files &&
  if [ ! -z "$ODIN_member" ]
  then
    mv "$ODIN_member" extract_dir
  fi
)
>WARNINGS 2>&1

if [ $? != 0 ]
then
  mv WARNINGS ERRORS
fi
