#!/bin/sh

if [ -n "$ODINVERBOSE" ]
then
  echo echo "#!/bin/sh" &&
  echo echo "$*"
  
fi
(
  echo "#!/bin/sh" &&
  echo "$*"
) > java.launcher &&
chmod ugo+x java.launcher
