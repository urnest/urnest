#!/bin/sh

set -e
py_cov_check=$1;shift
py=$1;shift
cov_check_warn=$1;shift
py_cov=$1;shift
if [ -n "$py_cov_check_warn" ]
then
  py_cov_check_warn="--cov-check-warn"
fi
if [ -z "$py_cov" ]
then
  echo "ERROR: +py-cov option is required for :py-cov-checked" >ERRORS
  exit 0
fi
if [ "$ODINVERBOSE" != "" ]
then
  echo ${ODINRBSHOST}env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHONPATH" python3 "$py_cov_check" $cov_check_warn $py_cov $py
fi
if ! env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHONPATH" python3 "$py_cov_check" $cov_check_warn $py_cov $py 2>WARNINGS
then
  mv WARNINGS ERRORS
fi
exit 0
