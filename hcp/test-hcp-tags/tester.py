#!/usr/bin/env python

import sys
import os.path

import json
def toJson(x):
    return json.dumps(x,sort_keys=True,indent=4,separators=(',',': '),
                      cls=JSONEncoder)
def fromJson(x):
    return json.loads(x)

def normValue(x,d):
    '''x like [{"f":"files/x","l":3}]'''
    return sorted([ {"f":os.path.join(d,_["f"]),"l":_["l"]} for _ in x])

got=fromJson(file(sys.argv[1]).read())
expect=fromJson(file(sys.argv[2]).read())
d=os.path.dirname(os.path.abspath(sys.argv[2]))
expect=dict([ (_[0],normValue(_[1],d)) for _ in expect.items()])
assert got==expect, 'got:\n{got!r}\nexpected:\n{expect!r}'.format(**vars())
