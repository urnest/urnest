#!/usr/bin/env python3
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

import traceback
import sys
import string

class FileAndLine(object):
    def __init__(self,file=None,line=None,readable=True):
        self.file=file
        self.line=line
        self.readable=readable
        pass
    def setTo(self,file,line):
        self.file=file
        self.line=line
    def __str__(self):
        if self.file:
            return '{file}:{line}: '.format(**self.__dict__)
        return ''
    pass

class Xn:
    """Capture cause and context.
    """
    def __init__(self, cause):
        '''cause is convertable to a string using str'''
        ''' e.g. cause can be an exception or a string like "file not found"'''
        '''cause can also have a readableRepr() method, in which case that'''
        '''will be used by readableRepr() below'''
        self.cause = (cause,FileAndLine()) # file,line set below
        self.context = [] # (text,FileAndLine)
        pass

    def __str__(self):
        '''programmer friendly format, each context and cause includes
        file and line, and intermediate stack entries are included
        - see unit test below for example'''
        result = ''
        x = ''.join([
            '{fl}failed to {s} because\n'.format(**vars())
            for s,fl in reversed(self.context)])
        y = '{cause[1]}{cause[0]}'.format(**vars(self))
        return x+y

    def readableRepr(self):
        '''human (non-programmer) readable representation, omitting file
        and line, omitting intermediate stack entries, and producing a
        proper sentence i.e. capitalised and ending in full stop
        - see unit test below for example'''
        result = ''
        x = ''.join([
            'failed to {s} because\n'.format(**vars())
            for s,fl in reversed(self.context)
            if fl.readable])
        try:
            y=self.cause[0].readableRepr()
            assert isinstance(y,str)
        except:
            y = str(self.cause[0])
            pass
        return capitalise(x+y+'.')
    pass

def readableRepr(e):
    if isinstance(e,Xn): return e.readableRepr()
    return str(e)

def capitalise(s):
    if s and s[0]!=s[0].upper():
        return s[0].upper()+s[1:]
    return s

def inContext(context, exceptionInfo=None):
    """Make a Xn that includes exception info and context.
    If exceptionInfo[1] is already a Xn just add context,
    otherwise use exceptionInfo as cause for a new Xn.

    exceptionInfo is as returned by sys.exc_info()
    """
    if exceptionInfo is None: exceptionInfo=sys.exc_info()
    exceptionType,r,traceBack=exceptionInfo

    if not isinstance(r,Xn):
        #build new exception type derived from both original and Xn
        name=exceptionType.__name__
        def init(self,v):
            Xn.__init__(self,v)
            self.__dict__.update(v.__dict__)
            pass
        def str_(self):
            return Xn.__str__(self)
        def readableRepr(self):
            return Xn.readableRepr(self)
        r=type('Xn{name}'.format(**vars()),
               (Xn,exceptionType),
               {
                   '__init__':init,
                   '__str__':str_,
                   'readableRepr':readableRepr
               })(r)
        pass
    
    st=traceback.extract_tb(traceBack)
    # fill in most recent file,line (latest context or cause if no context)
    f,l=st[-1][0:2]
    if r.context:
        r.context[-1][1].setTo(f,l)
    else:
        r.cause[1].setTo(f,l)
        pass
    # add context entries for any in-between stack entries
    newContext=[(text,FileAndLine(file,line,False))
                for file,line,fname,text in reversed(st[0:-1])]
    r.context.extend(newContext)
    # add the supplied context, with unknown file and line
    r.context.append((context,FileAndLine()))
    return r


def firstLineOf(x):
    return str(x).split('\n')[0]

def desentence(s):
    if s.endswith('.'): s=s[:-1]
    return s[0:1].lower()+s[1:]

def indent(prefix,s):
    '''prefix all but first line of s by specified prefix'''
    return s.replace('\n','\n'+prefix)

class AllFailed(Exception):
    def __init__(self,causes):
        self.causes=causes
        pass
    def __str__(self):
        return ', and\n'.join([str(cause) for cause in self.causes])
    def readableRepr(self):
        return '; and\n'.join(['- '+
                               indent('  ',desentence(readableRepr(cause)))
                               for cause in self.causes])
    pass

if __name__== '__main__':
    # unit test
    import assert_
    def test1():
        def a():
            raise Exception('fred')
        def b():
            a()
            pass
        def c():
            b()
            pass
        try:
            c()
        except:
            e=inContext('jock')
            assert_.equal(e.readableRepr(),'''\
Failed to jock because
fred.\
''')
            assert_.equal(str(e),'''\
failed to jock because
./xn.py:156: failed to c() because
./xn.py:153: failed to b() because
./xn.py:150: failed to a() because
./xn.py:148: fred''')
            pass
        pass

    class E(Exception):
        def __str__(self):
            return 'deadline reached'
        pass
    
    def test2():
        try:
            try:
                raise E()
            except:
                raise inContext('await message')
            pass
        except E as e:
            assert_.equal(readableRepr(e),'''\
Failed to await message because
deadline reached.''')
            pass
        pass
    
    def test3():
        try:
            try:
                try:
                    raise E()
                except:
                    raise inContext('await message')
                pass
            except E as e:
                try:
                    raise Exception('open failed')
                except:
                    raise AllFailed([e,inContext('read default file')])
                pass
            pass
        except:
            assert_.equal(readableRepr(inContext('get content')),'''\
Failed to get content because
- failed to await message because
  deadline reached; and
- failed to read default file because
  open failed.''')
            pass
        pass
    
    tests=[var for name,var in sorted(vars().items())
           if name.startswith('test') and callable(var)]
    for t in tests: t()
    n=len(tests)
    print('PASS - {n} steps'.format(**vars()))
    pass
