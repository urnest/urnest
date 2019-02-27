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
def equal(a,b):
    assert a==b, '{a!r} != {b!r}'.format(**vars())
    pass

class Assert():
    def __init__(self,x):
        self.x=x
        pass
    def isInstanceOf(self,y):
        if not isinstance(self.x,y):
            xc=self.x.__class__.__name__
            yc=y.__name__
            raise Exception('{self.x!r} is not an instance of class {yc} (it is of class {xc})'.format(**vars()))
        return self.x
    def isGreaterThan(self,y):
        if not self.x > y:
            raise Exception('{self.x!r} is not greater than {y!r}'.format(**vars()))
        return self.x
    def isLessThan(self,y):
        if not self.x < y:
            raise Exception('{self.x!r} is not less than {y!r}'.format(**vars()))
        return self.x
    def startsWith(self,y):
        if not self.x[0:len(y)]==y:
            raise Exception('{self.x!r} does not start with {y!r}'.format(**vars()))
        return self.x
    def isNotIn(self,y):
        if self.x in y:
            raise Exception('{self.x!r} is in {y!r}'.format(**vars()))
        pass
    def isIn(self,y):
        if not self.x in y:
            raise Exception('{self.x!r} is not in {y!r}'.format(**vars()))
        pass
    def __lt__(self,y):
        if not self.x < y:
            raise Exception('{self.x!r} is not less than {y!r}'.format(**vars()))
        return self.x
    def __le__(self,y):
        if not self.x <= y:
            raise Exception('{self.x!r} is not less than or equal to {y!r}'.format(**vars()))
        return self.x
    def __eq__(self,y):
        if not self.x == y:
            raise Exception('{self.x!r} is not equal to {y!r}'.format(**vars()))
        return self.x
    def __ne__(self,y):
        if self.x == y:
            raise Exception('{self.x!r} is equal to {y!r}'.format(**vars()))
        return self.x
    def __gt__(self,y):
        if not self.x > y:
            raise Exception('{self.x!r} is not greater than {y!r}'.format(**vars()))
        return self.x
    def __ge__(self,y):
        if not self.x >= y:
            raise Exception('{self.x!r} is not greater than or equal to {y!r}'.format(**vars()))
        return self.x
    pass

if __name__=='__main__':
    # unit test
    try:
        Assert('fred').isInstanceOf(int)
    except Exception as e:
        assert str(e)=="'fred' is not an instance of class int (it is of class str)",str(e)
        pass
    else:
        assert False
        pass
    pass

