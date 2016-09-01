#! /bin/sh

ODIN_fld=$1;shift;
ODIN_exe=$1;shift;

if [ "$ODIN_exe" != "" ] ; then 
  compiler=$ODIN_exe;
elif [ -n "$ODIN_FLUID" ]
then
  x=`PATH="$ODIN_FLUID_PATH" /usr/bin/which "$ODIN_FLUID" 2>&1`
  if [ $? != 0 ] ; then
      ( echo "Error: Failed to locate executable named by ODIN_FLUID on path specified by ODIN_FLUID_PATH (specified when Odin cache was created as '$ODIN_FLUID' and '$ODIN_FLUID_PATH' respectively) because"
      echo "$x" ) >&2
      exit 1
  fi
  compiler=$ODIN_FLUID
fi

b=`basename "$ODIN_fld"`
ln -s "$ODIN_fld" "$b"
if [ "$ODINVERBOSE" != "" ] ; then
   echo ${ODINRBSHOST}$compiler -c "$b"; fi

PATH="$ODIN_FLUID_PATH" \
LD_LIBRARY_PATH="$ODIN_FLUID_LD_LIBRARY_PATH" \
$compiler -c "$b" 2>MSGS
ok=$?
test $ok = 0 || {
  if egrep -s -f $ODIN_abort MSGS; then
         cat MSGS
         exit 1; fi
       mv MSGS ERRORS  
       echo "$compiler failed." >>ERRORS
       exit 0; }

egrep 'arning' MSGS >>WARNINGS
egrep -v 'arning' MSGS

input=`expr "$b" : '\([^/]*\)[.][^./]*'`
if [ -f $input.h ] ; then mv $input.h h; fi
if [ -f $input.cxx ] ; then mv $input.cxx cxx; fi

exit 0
