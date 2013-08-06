#!/bin/sh
ODIN_FILE=$1;shift; ODIN_DIR=$1; shift;
ODIN_incsp=$1;shift; ODIN_ignore=$1;shift;

if [ "$ODINVERBOSE" != "" ] ; then
   echo ${ODINRBSHOST}scan_for_includes `basename $ODIN_FILE`; fi

incsp=""
if [ "$ODIN_incsp" != "" ] ; then incsp="`cat $ODIN_incsp`"; fi
incsp="$incsp $ODIN_CXX_I"
for header in $incsp; do
   case $header in /*);; *)
      echo "Search path entry must be absolute: $header" >>ERRORS;;
      esac; done

sed -n -e '/^[ 	]*#[ 	]*include/s/^[^"]*"\([^"]*\)".*$/\1/w local' \
       -e '/^[ 	]*#[ 	]*include/s/^[^<]*<\([^>]*\)>.*$/\1/w global' \
   <$ODIN_FILE 2>>ERRORS

touch hash_inc.view_desc
dirs=$incsp
for name in `cat local`; do
   case $name in
      /* )
         echo "'$name'" >>hash_inc.view_desc
         echo "=''" >>hash_inc.view_desc;;
      * )
         for header in $ODIN_DIR $dirs; do
	    echo "'$header/$name'" >>hash_inc.view_desc; done
         echo "=''" >>hash_inc.view_desc;; esac; done \
 || echo 'C++ include scanning failed' >>ERRORS

for name in `cat global`; do
   case $name in
      /* )
         echo "'$name'" >>hash_inc.view_desc
         echo "=''" >>hash_inc.view_desc;;
      * )
         for header in $dirs; do
	    echo "'$header/$name'" >>hash_inc.view_desc; done
         echo "=''" >>hash_inc.view_desc;; esac; done \
 || echo 'C++ include scanning failed' >>ERRORS

[ ! -f hash_inc.view_desc ] || (
   [ "$ODIN_ignore" = "" ] || (
      egrep -v -f "$ODIN_ignore" <hash_inc.view_desc >tmp &&
      mv tmp hash_inc.view_desc; 
   ) &&
   [ "$ODIN_IGNORE" = "" ] || (
      egrep -v -e "$ODIN_IGNORE" <hash_inc.view_desc >tmp
      mv tmp hash_inc.view_desc; 
   )
) || echo 'adjust for $ODIN_IGNORE or +ignore failed'

exit 0
