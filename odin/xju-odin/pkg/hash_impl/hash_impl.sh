#!/bin/bash

join(){
  joiner=$1 && shift &&
  if [ -n "$1" ]
  then
    result=$1 && shift &&
    while [ -n "$1" ]
    do
      result="${result}${joiner}$1" &&
      shift
    done
    echo "$result"
  fi
}

ODIN_FILE=$1  && shift &&
ODIN_DIR=$1   && shift &&
ODIN_incsp=$1 && shift &&
ODIN_implsp=$1 && shift &&
ignore=$(join '|' $(cat $1)) && shift &&
suffixes=$(cat $1) && shift &&

if [ -z "$suffixes" ]
then
  suffixes="$ODIN_O_SRC_SUFFIXES"
fi &&

incsp="" &&
if [ "$ODIN_incsp" != "" ] ; then incsp="`cat $ODIN_incsp`"; fi &&
incsp="$incsp $ODIN_CXX_I" &&
for dir in $incsp; do
   case $dir in 
      /* )
         ;; 
      * )
         echo "Search path entry must be absolute: $dir" >>ERRORS &&
         false ;;
   esac; 
done &&

implsp="" &&
if [ "$ODIN_implsp" != "" ]
then
  implsp=$(cat $ODIN_implsp)
fi &&
for dir in $implsp; do
   case $dir in 
      /* )
         ;; 
      * )
         echo "+impl_sp $dir must be absolute" >>ERRORS &&
         false ;;
   esac; 
done &&

if [ "$ODINVERBOSE" != "" ] ; 
then
  echo ${ODINRBSHOST}scan_for_impls $ODIN_FILE in dirs $incsp
fi &&

sed -n -e '/^[ 	]*#[ 	]*include/s/^[^"]*"\([^"]*\)".*$/\1/w local' \
       -e '/^[ 	]*#[ 	]*include/s/^[^<]*<\([^>]*\)>.*$/\1/w global' \
   <$ODIN_FILE 2>>ERRORS &&

touch hash_impl.view_desc &&
dirs=$incsp &&
for name in `cat local | ( test -n "$ignore" && grep -E -v "$ignore" || cat )`
do
   case $name in
      /* )
         echo "'$name'" >>hash_impl.view_desc &&
         echo "=''" >>hash_impl.view_desc &&
         { ! expr $name : '.*[.]' >/dev/null || name=`expr $name : '\(.*\)[.][^.]*'`; } &&
         for suffix in $suffixes
         do
           echo "'${name}${suffix}'" >> hash_impl.view_desc
         done &&
         echo "=''" >>hash_impl.view_desc;;
      * )
         for dir in $ODIN_DIR $dirs
         do
	    echo "'$dir/$name'" >>hash_impl.view_desc
         done &&
         echo "=''" >>hash_impl.view_desc &&
         { ! expr $name : '.*[.]' >/dev/null || name=`expr $name : '\(.*\)[.][^.]*'`; } &&
         for dir in $ODIN_DIR $dirs
         do
            d=$(dirname "$dir/$name") &&
            if test -z "$implsp" || ( echo "$implsp"|grep -E -s "^$d\$" )
            then
              for suffix in $suffixes
              do
  	        echo "'$dir/${name}${suffix}'" >>hash_impl.view_desc
              done
            fi
         done &&
         echo "=''" >>hash_impl.view_desc;;
   esac
done || ( echo 'include scanning failed' ; false ) &&

for name in `cat global | ( test -n "$ignore" && grep -E -v "$ignore" || cat )`
do
   case $name in
      /* )
         echo "'$name'" >>hash_impl.view_desc &&
         echo "=''" >>hash_impl.view_desc &&
         { ! expr $name : '.*[.]' >/dev/null || name=`expr $name : '\(.*\)[.][^.]*'`; } &&
         for suffix in $suffixes
         do
           echo "'${name}${suffix}'" >> hash_impl.view_desc
         done &&
         echo "=''" >>hash_impl.view_desc;;
      * )
         for dir in $dirs 
         do
	    echo "'$dir/$name'" >>hash_impl.view_desc; 
         done &&
         echo "=''" >>hash_impl.view_desc &&
         { ! expr $name : '.*[.]' >/dev/null || name=`expr $name : '\(.*\)[.][^.]*'`; } &&
         for dir in $dirs
         do
            d=$(dirname "$dir/$name") &&
            if test -z "$implsp" || ( echo "$implsp"|grep -E -s "^$d\$" )
            then
              for suffix in $suffixes
              do
  	        echo "'$dir/${name}${suffix}'" >>hash_impl.view_desc
              done
            fi
         done &&
         echo "=''" >>hash_impl.view_desc;; 
   esac; 
done || ( echo 'C++ include scanning failed'; false )
