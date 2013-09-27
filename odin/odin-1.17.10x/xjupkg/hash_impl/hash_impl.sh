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

impl_base="" &&
file_base=$(basename $ODIN_FILE) &&
for suffix in $suffixes
do
  #use first matching suffix so eg suffix list _win.cxx .cxx
  #turns x_win.cxx into x
  #
  if [ -z "$impl_base" ] && expr $file_base : ".*$suffix" >/dev/null
  then
    impl_base=$(expr $file_base : "\(.*\)$suffix")
  fi
done &&

touch hash_impl.view_desc &&
dirs=$incsp &&
for name in `cat local | ( test -n "$ignore" && egrep -v "$ignore" || cat )`
do
   # to avoid a dependcy cycle, we ignore the #include "x.h" from
   # #include "x.cc", which is complicated by the suffixes, so
   # we assume x.h looks like x.<something> and that x.cc looks like
   # x<o_src_suffix>, we add the "."s so that eg x_y_z.h doesn't
   # get discarded by x.cc
   #
   name_base=$(basename "$name") &&
   { test -n "$impl_base" && 
     expr "$name_base." : "$impl_base[.]" >/dev/null; } ||
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
            if test -z "$implsp" || ( echo "$implsp"|egrep -s "^$d\$" )
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

for name in `cat global | ( test -n "$ignore" && egrep -v "$ignore" || cat )`
do
   name_base=$(basename "$name") &&
   { test -n "$impl_base" && 
     expr "$name_base." : "$impl_base[.]" >/dev/null ; } ||
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
            if test -z "$implsp" || ( echo "$implsp"|egrep -s "^$d\$" )
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
