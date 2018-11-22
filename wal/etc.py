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

def toJson(x,cls=json.JSONEncoder):
    return json.dumps(x,sort_keys=True,indent=4,separators=(',',': '),
                      cls=cls)
def fromJson(x):
    return json.loads(x)

from xn import inContext

class Scope:
    def __init__(self,description,
                 log=lambda s: print('INFO: {s}'.format(**vars()))):
        self.description=descriptions
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

