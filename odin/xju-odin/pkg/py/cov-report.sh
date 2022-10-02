#!/bin/sh

set -x &&
cov="$1" && shift &&

if [ "$ODINVERBOSE" != "" ]
then
  echo ${ODINRBSHOST}
fi
mkdir py3.cov.report
env - LD_LIBRARY_PATH="$ODIN_EXEC_LD_LIBRARY_PATH" PATH="$ODIN_EXEC_PATH" PYTHONPATH="$PYPATH:$ODIN_PYTHONPATH" python3 -c "import trace;import pickle;counts,b,c=pickle.load(open('$cov','rb'));trace.CoverageResults(counts).write_results(summary=True,coverdir='./py3.cov.report')" > py3.cov.summary 2>WARNINGS || mv WARNINGS ERRORS
