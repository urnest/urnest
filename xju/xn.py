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
from typing import Sequence,Callable,Literal,Dict,List,Tuple,Any,Type
from types import TracebackType

class FileAndLine(object):
    def __init__(self,file:str|None=None,line:int|None=None,readable:bool=True)->None:
        self.file=file
        self.line=line
        self.readable=readable
        pass
    def setTo(self,file:str,line:int)->None:
        self.file=file
        self.line=line
        pass
    def __str__(self)->str:
        if self.file:
            return '{file}:{line}: '.format(**self.__dict__)
        return ''
    pass

class Xn:
    """Capture cause and context.
    """
    def __init__(self, cause:object)->None:
        '''cause is convertable to a string using str'''
        ''' e.g. cause can be an exception or a string like "file not found"'''
        '''cause can also have a xju_xn_readable_repr() method, in which case that'''
        '''will be used by xju_xn_readable_repr() below'''
        self.cause = (cause,FileAndLine()) # file,line set below
        self.context:List[Tuple[str,FileAndLine]] = [] # (text,FileAndLine)
        pass

    def __str__(self)->str:
        '''programmer friendly format, each context and cause includes
        file and line, and intermediate stack entries are included
        - see unit test below for example'''
        result = ''
        x = ''.join([
            '{fl}failed to {s} because\n'.format(**vars())
            for s,fl in reversed(self.context)])
        m = str(self.cause[0]) or type(self.cause[0]).__name__
        y = f'{self.cause[1]}{m}'
        return x+y

    def xju_xn_readable_repr(self)->str:
        '''human (non-programmer) readable representation, omitting file
        and line, omitting intermediate stack entries, and producing a
        proper sentence i.e. capitalised and ending in full stop
        - see unit test below for example'''
        result = ''
        x:str = ''.join([
            'failed to {s} because\n'.format(**vars())
            for s,fl in reversed(self.context)
            if fl.readable])
        if hasattr(self.cause[0], 'xju_xn_readable_repr'):
            y:str=self.cause[0].xju_xn_readable_repr()
        else:
            # some python library exceptions have pretty useless str()
            match self.cause[0]:
                case KeyError():
                    y=repr(self.cause[0])
                    pass
                case TimeoutError():
                    y=type(self.cause[0]).__name__
                    pass
                case _:
                    y = str(self.cause[0]) or type(self.cause[0]).__name__
                    pass
            pass
        return capitalise(x+y+'.')
    pass

def readable_repr(e:BaseException)->str:
    r=getattr(e,'xju_xn_readable_repr',None)
    if callable(r):
        return str(r())
    return str(e)

def capitalise(s:str)->str:
    if s and s[0]!=s[0].upper():
        return s[0].upper()+s[1:]
    return s

def in_function_context(function:Callable,
                        vars:Dict[str,Any]={},
                        exceptionInfo:None|tuple[Type[BaseException],BaseException,TracebackType]=None,
                        fl:None|tuple[str,int]=None)->BaseException:
    """Make a Xn that includes exception info and context as first_para_of(f.__doc__).format(**vars()).

       - if exceptionInfo[1] is already a Xn just add context
       - otherwise use exceptionInfo as cause for a new Xn
       - exceptionInfo defaults to sys.exc_info()
    """
    if function.__doc__ is None:
        return in_context(f"{function.__name__}() (note function is missing doc-string!)",
                          exceptionInfo=exceptionInfo,
                          fl=fl)
    else:
        try:
            s=first_para_of(function.__doc__).format(**vars)
        except Exception:
            s=f"{function.__name__}() (note that function's doc-string {function.__doc__!r} is unformattable - check its params/vars v its doc string)"
            pass
        return in_context(s,
                          exceptionInfo=exceptionInfo,
                          fl=fl)

def in_context(context:str,
               exceptionInfo:None|tuple[Type[BaseException],BaseException,TracebackType]=None,
               fl:None|tuple[str,int]=None)->BaseException:
    """Make a Xn that includes exception info and context.

       - if exceptionInfo[1] is already a Xn just add context
       - otherwise use exceptionInfo as cause for a new Xn
       - exceptionInfo defaults to sys.exc_info()
    """
    if exceptionInfo is None:
        t, e, b = sys.exc_info()
        assert t is not None
        assert e is not None
        assert b is not None
        exceptionInfo=(t,e,b)
    exceptionType,r,traceBack=exceptionInfo

    if not isinstance(r,Xn):
        #build new exception type derived from both original and Xn
        name=exceptionType.__name__
        def init(self,v):
            Xn.__init__(self,v)
            for a in set.difference(set(dir(v)),set(dir(Xn))):
                try:
                    setattr(self,a,getattr(v,a))
                except AttributeError:
                    pass
                pass
            pass
        def str_(self)->str:
            return Xn.__str__(self)
        def xju_xn_readable_repr(self)->str:
            return Xn.xju_xn_readable_repr(self)
        r=type(name,
               (Xn,exceptionType),
               {
                   '__init__':init,
                   '__str__':str_,
                   'xju_xn_readable_repr':xju_xn_readable_repr
               })(r)
    
    st=[tuple(_) for _ in traceback.extract_tb(traceBack)]
    # fill in most recent file,line (latest context or cause if no context)
    f,l=st[-1][0:2]
    assert isinstance(r, Xn)
    if r.context:
        if not r.context[-1][1].file:
            r.context[-1][1].file=f
            pass
        if not r.context[-1][1].line:
            r.context[-1][1].file=l
            pass
    else:
        r.cause[1].setTo(f,l)
        pass
    # add context entries for any in-between stack entries
    newContext=[(text,FileAndLine(file,line,False))
                for file,line,fname,text in reversed(st[0:-1])]
    r.context.extend(newContext)
    # add the supplied context, with unknown file and line
    f2=fl[0] if fl else None
    l2=fl[1] if fl else None
    r.context.append( (context,FileAndLine(f2,l2)) )
    traceBack.tb_next=None
    return r


def first_line_of(x:Any)->str:
    '''return first non-empty line of str({x}) stripped of leading and trailing whitespace'''
    return str(x).strip().split('\n')[0].strip()

def first_para_of(x:Any)->str:
    '''return first paragraph of str({x}) stripped of leading and trailing whitespace
    with separate lines stripped and joined by a single space

    - paragraph ends at blank line'''
    lines=[_.strip() for _ in str(x).strip().split('\n')]+['']
    end=lines.index('')
    return ' '.join(lines[0:end])

def desentence(s:str)->str:
    '''remove any trailing '.' and down-case first characters of {s}'''
    if s.endswith('.'): s=s[:-1]
    return s[0:1].lower()+s[1:]

def indent(prefix:str,s:str)->str:
    '''prefix all but first line of s by specified prefix'''
    return s.replace('\n','\n'+prefix)

class AllFailed(Exception):
    def __init__(self,causes:Sequence[BaseException])->None:
        self.causes=causes
        pass
    def __str__(self)->str:
        return ', and\n'.join([str(cause) for cause in self.causes])
    def xju_xn_readable_repr(self)->str:
        return '; and\n'.join(['- '+
                               indent('  ',desentence(readable_repr(cause)))
                               for cause in self.causes])
    pass

