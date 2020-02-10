#!/usr/bin/env python3

# Copyright (c) 2018 Trevor Taylor
# 
# Permission to use, copy, modify, and/or distribute this software for
# any purpose with or without fee is hereby granted, provided that all
# copyright notices and this permission notice appear in all copies.
# 
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
import json
import sys
import io

def toJson(x,cls=json.JSONEncoder):
    return json.dumps(x,sort_keys=True,indent=4,separators=(',',': '),
                      cls=cls)
def fromJson(x):
    return json.loads(x)

from xju.xn import inContext

class Scope:
    def __init__(self,description,
                 log=lambda s: print('INFO: {s}'.format(**vars()))):
        self.description=description
        self.log=log
        self.result_=None
        log('+ '+self.description)
        pass
    def __enter__(self):
        return self
    def __exit__(self,eType,eVal,eTrc):
        self.log('- '+self.description+' = '+repr(self.result_))
        description=self.description
        self.description=None
        if eType:
            raise inContext(description, (eType,eVal,eTrc))
        return False
    def __del__(self):
        if self.description:
            self.log('- '+self.description+' = '+repr(self.result_))
            pass
        pass
    def result(self,result):
        self.result_=result
        return result
    pass

if __name__=='__main__':
    from assert_ import Assert
    sys.stdout=io.StringIO()
    with Scope('do some stuff') as scope:
        pass
    Assert(sys.stdout.getvalue())=='''\
INFO: + do some stuff
INFO: - do some stuff = None
'''
    sys.stdout=io.StringIO()
    def f():
        with Scope('do some stuff') as scope:
            return scope.result(77)
            pass
        pass
    Assert(f())==77
    Assert(sys.stdout.getvalue())=='''\
INFO: + do some stuff
INFO: - do some stuff = 77
'''
    def f():
        with Scope('do some stuff') as scope:
            raise Exception('did not work')
        pass
    try:
        f()
    except Exception as e:
        Assert(e.readableRepr())=='''\
Failed to do some stuff because
did not work.'''
    else:
        assert False,'should not reach'
    pass
