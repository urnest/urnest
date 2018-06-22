#!/bin/sh
ODIN_py=$1 && shift &&
ODIN_DIR=$1 && shift &&
ODIN_py_sp=$1 && shift &&
ODIN_ignore=$1 && shift &&

if [ "$ODINVERBOSE" != "" ]
then
    echo ${ODINRBSHOST}scan_for_imports `basename $ODIN_py` &&
    set -x
fi &&

py_sp="" &&
if [ "$ODIN_py_sp" != "" ]
then
    py_sp="`cat $ODIN_py_sp`"
fi &&
for d in $py_sp
do
    case $d in
	/*);;
	*)
	    echo "+py_sp entry must be absolute: $d" >>ERRORS;;
    esac;
done &&
envDirs=$(echo $ODIN_PYTHONPATH | sed -e 's/:/ /g') &&
for d in $envDirs
do
    case $d in
	/*);;
	*)
	    echo "\$ODIN_PYTHONPATH entry must be absolute: $d" >>ERRORS;;
    esac;
done &&

sed -n \
    -e '/^[ \t]*import[ \t]/s/^[ \t]*import[ \t][ \t]*\([a-ZA-Z_0-9.]*\).*$/\1/w a' \
    -e '/^[ \t]*from[ \t]/s/^[ \t]*from[ \t][ \t]*\([a-ZA-Z_0-9.]*\)[ \t][ \t]*import[ \t].*$/\1/w a' \
   <$ODIN_py 2>>ERRORS &&

touch py_import.view_desc &&
dirs=$py_sp &&

for name in `cat a`
do
    #REVISIT need correct handling of leading dots
    if echo $name | egrep -q '^[.]'
    then
	leadingDots=$(echo $name | sed -e 's=^\([.]*\).*=\1=g') &&
	rest=$(echo $name | sed -e 's=^[.]*\(.*\)=\1=g') &&
	self=$(basename .py $ODIN_py) &&
	echo "
    nameDirs=$(echo $name | sed -e 's=[.]=/=g') &&
    for d in $ODIN_DIR $dirs $envDirs
    do
	echo "'$d/$name.py'" >>py_import.view_desc &&
	echo "'$d/$name/__init__.py'" >>py_import.view_desc
    done &&
    echo "=''" >>py_import.view_desc
done &&

if [ "$ODIN_ignore" != "" ]
then
    egrep -v -f "$ODIN_ignore" <py_import.view_desc >tmp &&
    mv tmp py_import.view_desc ||
	echo "adjust for +ignore \"$(cat $ODIN_ignore)\" failed " >>ERRORS
fi &&

if [ "$ODIN_PY_IGNORE" != "" ]
then
    egrep -v -e "$ODIN_PY_IGNORE" <py_import.view_desc >tmp &&
    mv tmp py_import.view_desc ||
	echo "adjust for \$ODIN_PY_IGNORE sed expression \"$ODIN_PY_IGNORE\" failed" >>ERRORS
fi
