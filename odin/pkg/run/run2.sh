#!/bin/sh

ODIN_file=$1;shift; ODIN_cmd=$1;shift;

if [ "$ODIN_cmd" != "" ] ; then
  cmd=`cat $ODIN_cmd`
fi

echo '#!/bin/sh' >run2
echo "\"$ODIN_file\"" $cmd "\"\$@\"" >>run2
chmod a+x run2

exit 0
