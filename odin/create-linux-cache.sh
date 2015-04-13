#!/bin/sh
if [ -z "$1" ]
then
  echo "specify cache location as only parameter" >&2
  exit 1
fi

d=`dirname "$0"`
if ! expr "$d" : "^/"
then
  d="`pwd`/$d"
fi

export ODIN_JAVA_OUTPUT_DIRECTORY="$1/classes"
rm -rf "$ODIN_JAVA_OUTPUT_DIRECTORY" &&
ODIN="$1" \
ODINPATH="$d/odin-1.17.10x/xjupkg" \
ODIN_JAVA_PATH=/usr/bin \
ODIN_SQL_DB=xxx \
ODIN_SQL_USER=root \
ODIN_SQL_PASSWORD=wirephone \
ODIN_SQL_PATH=/usr/bin:"$d/pkg/sql" \
ODIN_SQL_CLIENT=mysql_client.sh \
ODIN_TAR_PATH=/bin \
ODIN_RANLIB=ranlib \
ODIN_CC_FLAGS=-g \
ODIN_CXX_LD_EXTRA_LIBS=-lpthread \
odin -R </dev/null &&
mkdir "$ODIN_JAVA_OUTPUT_DIRECTORY"
