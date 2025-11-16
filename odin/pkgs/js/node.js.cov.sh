#!/bin/sh
d=$(cd $(dirname "$0") && pwd)

js=$1;shift
js_flags=$1;shift;

cmd="$ODIN_NODE_JS --test --experimental-test-coverage"
if [ -n "$js_flags" ]; then cmd="$cmd $(cat $js_flags)"; fi

verbose(){
  test -z "$ODINVERBOSE"||echo "$@"
}
verbose "$cmd $js"
(
export PATH="$ODIN_NODE_JS_PATH"
if ! which "$ODIN_NODE_JS" >/dev/null
then
  echo "Error: node.js executable \"$ODIN_NODE_JS\" (specified as ODIN_NODE_JS when the Odin cache was created) is not executable via path \"$ODIN_NODE_JS_PATH\" (specified when Odin cache was created as '$ODIN_NODE_JS_PATH') - see $d/js.dg" >&2
  false
fi &&

if x=$(NODE_V8_COVERAGE=. $cmd $js)
then
  echo "$x" | grep "/$(basename $js) " | (
    read a b c d e f g h i lines_not_covered
    echo -n "[$lines_not_covered]"
  ) > node.js.cov
else
  echo "$x" >&2
  false
fi

) 2>WARNINGS || mv WARNINGS ERRORS
