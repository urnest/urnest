#!/usr/bin/env python

import sys
import os.path

import json
def toJson(x):
    return json.dumps(x,sort_keys=True,indent=4,separators=(',',': '),
                      cls=JSONEncoder)
def fromJson(x):
    return json.loads(x)

got=fromJson(file(sys.argv[1]).read())
expect=fromJson(file(sys.argv[2]).read())
d=os.path.dirname(os.path.abspath(sys.argv[2]))
expect=dict([ (_[0],[os.path.join(d,_[1][0]),_[1][1]]) for _ in expect.items()])

assert got==expect, (repr(got),repr(expect))
