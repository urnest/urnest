# coding: utf-8

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
# Assert reports values of both terms where assertion fails, without trying
# to be fancy.
#
import re
from typing import Union

class Assert():
    def __init__(self,x):
        self.x=x
        pass
    def isInstanceOf(self,y):
        if not isinstance(self.x,y):
            xc=self.x.__class__.__name__
            yc=y.__name__
            raise Exception(f'{self.x!r} is not an instance of class {yc} (it is of class {xc})')
        return self.x
    def isNotInstanceOf(self,y):
        if isinstance(self.x,y):
            xc=self.x.__class__.__name__
            yc=y.__name__
            raise Exception(f'{self.x!r} is unexpectedly an instance of class {yc} (it is of class {xc})')
        return self.x
    def isSubclassOf(self,y):
        if not issubclass(self.x,y):
            yc=y.__name__
            raise Exception(f'{self.x!r} is not an subclass of class {yc}')
        return self.x
    def isNotSubclassOf(self,y):
        if issubclass(self.x,y):
            yc=y.__name__
            raise Exception(f'{self.x!r} is unexpectedly an subclass of class {yc}')
        return self.x
    def hasAttr(self,y:str):
        if not hasattr(self.x,y):
            raise Exception(f'{self.x!r} has no {y!r} attribute')
    def doesNotHaveAttr(self,y:str):
        if hasattr(self.x,y):
            raise Exception(f'{self.x!r} unexpectedly has {y!r} attribute')
    def startsWith(self,y):
        if not self.x[0:len(y)]==y:
            raise Exception(f'{self.x!r} does not start with {y!r}')
        return self.x
    def startswith(self,y):
        return self.startsWith(y)
    def starts_with(self,y):
        return self.startsWith(y)
    def endsWith(self,y):
        if not self.x[-len(y):]==y:
            raise Exception(f'{self.x!r} does not end with {y!r}')
        return self.x
    def endswith(self,y):
        return self.endsWith(y)
    def ends_with(self,y):
        return self.endsWith(y)
    def contains(self,y):
        if not y in self.x:
            raise Exception(f'{self.x!r} does not contain {y!r}')
        return self.x
    def isNotIn(self,y):
        if self.x in y:
            raise Exception(f'{self.x!r} is unexpectedly in {y!r}')
        pass
    def isIn(self,y):
        if not self.x in y:
            raise Exception(f'{self.x!r} is not in {y!r}')
        pass
    def matches(self,x:Union[str,re.Pattern]):
        c:re.Pattern
        pattern:str
        if isinstance(x,re.Pattern):
            pattern=x.pattern
            c=x
        elif isinstance(x,str):
            pattern=x
            c=re.compile(pattern)
        else:
            assert False, '?'
        if not c.match(self.x):
            raise Exception(f'{self.x!r} does not match regular expression {pattern!r}')
        pass
    def doesNotMatch(self,x:Union[str,re.Pattern]):
        c:re.Pattern
        pattern:str
        if isinstance(x,re.Pattern):
            pattern=x.pattern
            c=x
        elif isinstance(x,str):
            pattern=x
            c=re.compile(pattern)
        else:
            assert False, '?'
        if c.match(self.x):
            raise Exception(f'{self.x!r} unexpectedly matches regular expression {pattern!r}')
        pass
    def __lt__(self,y):
        if type(y) is not type(self.x):
            xt=type(self.x)
            yt=type(y)
            raise Exception(f'{y} is a {yt}, not a {xt} (which is what {self.x} is)')
        if not self.x < y:
            raise Exception(f'{self.x!r} is not less than {y!r}')
        return self.x
    def __le__(self,y):
        if type(y) is not type(self.x):
            xt=type(self.x)
            yt=type(y)
            raise Exception(f'{y} is a {yt}, not a {xt} (which is what {self.x} is)')
        if not self.x <= y:
            raise Exception(f'{self.x!r} is not less than or equal to {y!r}')
        return self.x
    def __eq__(self,y):
        if type(y) is not type(self.x):
            xt=type(self.x)
            yt=type(y)
            raise Exception(f'{y} is a {yt}, not a {xt} (which is what {self.x} is)')
        if not self.x == y:
            raise Exception(f'{self.x!r} is not equal to {y!r}')
        return self.x
    def __ne__(self,y):
        if type(y) is not type(self.x):
            xt=type(self.x)
            yt=type(y)
            raise Exception(f'{y} is a {yt}, not a {xt} (which is what {self.x} is)')
        if self.x == y:
            raise Exception(f'{self.x!r} is unexpectedly equal to {y!r}')
        return self.x
    def __gt__(self,y):
        if type(y) is not type(self.x):
            xt=type(self.x)
            yt=type(y)
            raise Exception(f'{y} is a {yt}, not a {xt} (which is what {self.x} is)')
        if not self.x > y:
            raise Exception(f'{self.x!r} is not greater than {y!r}')
        return self.x
    def __ge__(self,y):
        if type(y) is not type(self.x):
            xt=type(self.x)
            yt=type(y)
            raise Exception(f'{y} is a {yt}, not a {xt} (which is what {self.x} is)')
        if not self.x >= y:
            raise Exception(f'{self.x!r} is not greater than or equal to {y!r}')
        return self.x
    def is_(self,y):
        if not self.x is y:
            raise Exception(f'{self.x} is not the same object as {y}')
        return self.x
    def isNot(self,y):
        if self.x is y:
            raise Exception(f'{self.x} is the same object as {y}')
        return self.x
    pass
