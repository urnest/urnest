#!/bin/bash
#
# Module
#
#    $RCSfile: moc.sh,v $ $Revision: 1.4 $
#
# Abstract
#
#    This script runs the Qt moc tool on a specified C++ header file
#    to produce a C++ source file.
#
#    Notes:
#
ODIN_source="$1";shift; 
ODIN_inc_prefix="$1";shift;

    typeset -a args

    x=`PATH="$ODIN_QT_MOC_PATH" /usr/bin/which "$ODIN_QT_MOC" 2>&1`
    if [ $? != 0 ] ; then
        ( echo "Error: Failed to locate executable named by ODIN_QT_MOC on path specified by ODIN_QT_MOC_PATH (specified when Odin cache was created as '$ODIN_QT_MOC_PATH' and '$ODIN_QT_MOC_PATH' respectively) because"
        echo "$x" ) >&2
        exit 1
    fi
    if [ -z "$ODIN_QTDIR" ] ; then
        echo "Error: \$ODIN_QTDIR not set. (Set it to locate the Qt distribution.)" >&2
        exit 1
    fi
    QTDIR="$ODIN_QTDIR"; export QTDIR

    if [ "$ODIN_inc_prefix" != "" ]
    then
        args[${#args[@]}]="-p"
        args[${#args[@]}]="$ODIN_inc_prefix"
    fi
    b=`/bin/basename "$ODIN_source"`
    /bin/ln -s "$ODIN_source" "$b"
    args[${#args[@]}]="$b"
    if [ "$ODINVERBOSE" != "" ] ; 
    then
	echo ${ODINRBSHOST}"$ODIN_QT_MOC" "${args[@]}";
    fi


    PATH="$ODIN_QT_MOC_PATH" "$ODIN_QT_MOC" "${args[@]}" -o moc.cxx 2>WARNINGS
    ok=$?
    test $ok = 0 || { 
        mv WARNINGS ERRORS; echo "moc failed" >>ERRORS; }
