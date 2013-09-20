#!/bin/sh
V="$ODINVERBOSE" &&

ols="$1" &&
{ which which || { echo "which not on PATH ($PATH)" && false; }; } &&
{ xargs=`which xargs` || { echo "xargs not on PATH ($PATH)" && false; }; } &&

{ test -z "$V" || echo "${ODINRBSHOST}cat \"$ols\" | PATH=\"$ODIN_AR_PATH\" \"$xargs\" \"$ODIN_AR\" qcs a"; } &&

cat "$ols" | PATH="$ODIN_AR_PATH" "$xargs" "$ODIN_AR" qcsv a 2>ERRORS &&
if [ "$ODIN_RANLIB" != "" ] ; 
then
   { test -z "$V" || echo "${ODINRBSHOST}\"$ODIN_RANLIB\" a"; } &&
   "$ODIN_RANLIB" a 2>>ERRORS
fi &&
mv ERRORS WARNINGS
