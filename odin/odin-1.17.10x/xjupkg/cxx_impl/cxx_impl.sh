#!/bin/sh

ODIN_FILE=$1;shift; ODIN_DIR=$1; shift;
ODIN_incsp=$1;shift; ODIN_impl=$1;shift;

if [ "$ODINVERBOSE" != "" ] ; then
   echo ${ODINRBSHOST}scan_for_impls `basename $ODIN_FILE`; fi

incsp=""
if [ "$ODIN_incsp" != "" ] ; then incsp="`cat $ODIN_incsp`"; fi
incsp="$incsp $ODIN_CXX_I"
for header in $incsp; do
   case $header in /*);; *)
      echo "Search path entry must be absolute: $header" >>ERRORS;;
      esac; done

impl=""
if [ "$ODIN_impl" != "" ] ; then impl="`cat $ODIN_impl`"; fi

( cat "$ODIN_FILE" $impl |
sed -n -e '/^[ 	]*#[ 	]*include/s/^[^"]*"\([^"]*\)".*$/\1/w local' \
       -e '/^[ 	]*#[ 	]*include/s/^[^<]*<\([^>]*\)>.*$/\1/w global' |
fgrep -v "`basename $ODIN_FILE`" ) 2>>ERRORS

touch impl.cxx.sm
if [ -n "$impl" ]
then
   for i in $impl
   do
      echo "$i" >> impl.cxx.sm
   done
fi

dirs=$incsp
for name in `cat local`
do
   case $name in
      /* )
         d=`dirname "$name"`
         if [ "$d" = "$ODIN_DIR" ]
         then
            b=`basename "$name"`
            echo "$d%$b.cxx.sm" >> impl.cxx.sm
         fi;;
      * )
         for header in $ODIN_DIR $dirs
         do
            x="$header/$name"
            d=`dirname "$x"`
            if [ "$d" = "$ODIN_DIR" ]
            then
               b=`basename "$name"`
               echo "$d%$b.cxx.sm" >> impl.cxx.sm
            fi
         done;;
   esac;
done 2>>ERRORS ||
echo 'C++ include scanning failed' >>ERRORS

for name in `cat global`
do
   case $name in
      /* )
         d=`dirname "$name"`
         if [ "$d" = "$ODIN_DIR" ]
         then
            b=`basename "$name"`
            echo "$d%$b.cxx.sm" >> impl.cxx.sm
         fi;;
      * )
         for header in $dirs
         do
            x="$header/$name"
            d=`dirname "$x"`
            if [ "$d" = "$ODIN_DIR" ]
            then
               b=`basename "$name"`
               echo "$d%$b.cxx.sm" >> impl.cxx.sm
            fi
         done;;
   esac;
done 2>>ERRORS ||
echo 'C++ include scanning failed' >>ERRORS

exit 0
