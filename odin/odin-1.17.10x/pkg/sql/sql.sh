#!/bin/sh
ODIN_sql=$1; shift

x=`PATH="$ODIN_SQL_PATH" /usr/bin/which "$ODIN_SQL_CLIENT" 2>&1`
if [ $? != 0 ] ; then
    ( echo "Error: Failed to locate executable \"$ODIN_SQL_CLIENT\" (specified when Odin cache was created as '$ODIN_SQL_CLIENT') on path specified by ODIN_SQL_PATH because"
      echo "$x" ) >&2
    exit 1
fi

if [ "$ODINVERBOSE" != "" ] ; 
then
   echo "${ODINRBSHOST} $ODIN_SQL_CLIENT $ODIN_SQL_DB $ODIN_SQL_USER *password* < $ODIN_sql" 2>&1
fi
PATH="$ODIN_SQL_PATH" "$ODIN_SQL_CLIENT" "$ODIN_SQL_DB" "$ODIN_SQL_USER" "$ODIN_SQL_PASSWORD" < "$ODIN_sql" >sql.stdout 2>&1 || {
  mv sql.output ERRORS;
}
