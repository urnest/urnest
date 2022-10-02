#!/bin/sh

covs=$(cat "$1") && shift &&
covmerge="$1" && shift &&

if [ "$ODINVERBOSE" != "" ]
then
  echo ${ODINRBSHOST}$covmerge agg.py3.cov $covs
fi
env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHONPATH" $covmerge agg.py3.cov $covs 2>WARNINGS || mv WARNINGS ERRORS
