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
from json import JSONDecodeError
import traceback
import sys
import string
from typing import Sequence,Callable,Literal,Dict,List,Tuple,Any,Type,overload
from types import TracebackType

def readable_repr(e:Exception)->str:
    """
    non-programmer friendly muli-line representation of {e}

    - forms a sentence if {e} contains context (see in_context/in_function_context)
    - otherwise str(e)
    """
    r=getattr(e,'xju_xn_readable_repr',None)
    if callable(r):
        return r()
    r2=getattr(e,'xju_xn',None)
    if isinstance(r2, Xn):
        return capitalise(f"{phrase_repr(e)}.")
    return str(e)

def phrase_repr(e:Exception)->str:
    """
    non-programmer friendly muli-line phrase representation of {e}

    - like readable_repr but not "sentenced"
    """
    r=getattr(e,'xju_xn_readable_repr',None)
    if callable(r):
        return r()
    r2=getattr(e,'xju_xn',None)
    if isinstance(r2, Xn):
        return r2.phrase_repr()
    return str(e)

def in_function_context(function:Callable,
                        vars:Dict[str,Any],
                        exceptionInfo:None|tuple[Type[BaseException],BaseException,TracebackType]=None,
                        fl:None|tuple[str,int]=None)->Exception:
    """
    like in_context(), building context built as first_para_of(function.__doc__).format(**vars()).

    Again this function must only be called from handlers of Exception, not BaseException.
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
               fl:None|tuple[str,int]=None)->Exception:
    """Make a exception that includes exception info and context.

       - if exceptionInfo[1] is already a Xn just add context
       - otherwise use exceptionInfo as cause for a new exception:
         - that inherits from exceptionInfo[1]'s class
         - that tries to copy all attribute values of exceptionInfo[1]
       - exceptionInfo defaults to sys.exc_info()

       Note that this function must only be called in handlers of Exception,
       not in handlers of BaseException.
    """
    if exceptionInfo is None:
        t, e, b = sys.exc_info()
        assert t is not None
        assert e is not None
        assert b is not None
        exceptionInfo=(t,e,b)
    exceptionType,r,traceBack=exceptionInfo

    assert isinstance(r, Exception), (type(r), r)

    xn = getattr(r, "xju_xn", None)
    if xn is None:
        try:
            xn_cause: XnGroupCause | XnCause
            args: tuple
            match r:
                case JSONDecodeError():
                    args = (r.msg, r.doc, r.pos)
                case _:
                    args = r.args
            match r:
                case ExceptionGroup():
                    xn_cause = XnGroupCause(r.message, r.exceptions)
                    pass
                    
                case Exception():
                    # some python library exceptions have pretty useless str()
                    match r:
                        case KeyError():
                            s = repr(r)  # capture in case it calls __str__
                            simple_phrase = lambda: s
                            pass
                        case TimeoutError():
                            t_r = type(r)  # capture before we override
                            simple_phrase = lambda: t_r.__name__
                            pass
                        case _:
                            r_str = r.__str__  # capture before we override
                            r_class_name = r.__class__.__name__
                            simple_phrase = lambda: (r_str() or r_class_name)
                            pass
                    xn_cause = XnCause(simple_phrase)
                    pass
                case _:
                    return r

            xn = Xn(xn_cause)

            def str_(_) -> str:
                return str(xn)
            # it would be easier if we could just:
            # setattr(r, "__str__", str_)
            # setattr(r, "xju_xn_readable_repr", xju_xn_readable_repr)
            # ... but __str__ can't be replaced in general, so
            # create a new type to get the effect, noting
            # ExceptionGroup and Exception are very different
            match r:
                case ExceptionGroup():
                    def new_(cls, *args):
                        self = r.__class__.__new__(cls, *args)
                        self.xju_xn = xn
                        return self
                    r=type(f"{exceptionType.__name__}WithContext",
                           (exceptionType,),
                           {
                               '__new__':new_,
                               '__str__':str_,
                           })(*args)
                case Exception():
                    def init_(self, v):
                        self.xju_xn = xn
                        # note we can't tell what arguments to pass to super().__init__
                        # so resort to copying as much as we can
                        for a in set.difference(set(dir(v)),set(dir(self)).union({'xju_xn'})):
                            try:
                                setattr(self,a,getattr(v,a))
                            except AttributeError:  #pragma NO COVER
                                pass
                            pass
                        pass
                    r=type(f"{exceptionType.__name__}WithContext",
                           (exceptionType,),
                           {
                               '__init__':init_,
                               '__str__':str_,
                           })(r)
        except Exception:
            return r
        pass
    if not isinstance(xn, Xn):
        return r

    st=[tuple(_) for _ in traceback.extract_tb(traceBack)]
    # fill in most recent file,line (latest context or cause if no context)
    f,l=st[-1][0:2]
    if xn.context:
        if not xn.context[-1][1].file:
            xn.context[-1][1].file=f
            pass
        if not xn.context[-1][1].line:
            xn.context[-1][1].line=l
            pass
    else:
        xn.cause.file_and_line.setTo(f,l)
        pass
    # add context entries for any in-between stack entries
    newContext=[(text,FileAndLine(file,line,False))
                for file,line,fname,text in reversed(st[0:-1])]
    xn.context.extend(newContext)
    # add the supplied context, with unknown file and line
    f2=fl[0] if fl else None
    l2=fl[1] if fl else None
    xn.context.append( (context,FileAndLine(f2,l2)) )
    return r

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

class XnCause:
    cause_phrase: Callable[[], str]
    file_and_line: FileAndLine

    def __init__(self, cause_phrase: Callable[[], str]) -> None:
        self.cause_phrase = cause_phrase
        self.file_and_line = FileAndLine() # set by in_context()
        pass

    def __str__(self)->str:
        """programmer friendly format, includes file and line"""
        return f'{self.file_and_line}{self.cause_phrase()}'

    def phrase_repr(self) -> str:
        """non-programmer friendly format - no file and line"""
        return self.cause_phrase()
    pass

class XnGroupCause:
    msg: str
    causes: Sequence[Exception]
    file_and_line: FileAndLine

    def __init__(self, msg: str, causes: Sequence[Exception]) -> None:
        self.msg = msg
        self.causes = causes
        self.file_and_line = FileAndLine() # set by in_context()
        pass

    def __str__(self)->str:
        """programmer friendly format, includes file and line"""
        return (
            (f"{self.file_and_line}{self.msg}:\n" if self.msg else "")+
            '\nAND\n'.join([
                '  '+indent('  ', f'{self.file_and_line}{str(cause) or type(cause).__name__}')
                for cause in self.causes
            ])
        )

    def phrase_repr(self) -> str:
        """non-programmer friendly format - no file and line"""
        return (
            (f"{self.msg}:\n" if self.msg else "")+
            '; and\n'.join('- '+indent('  ', phrase_repr(cause))
                           for cause in self.causes)
        )
    pass

class Xn:
    """Capture cause and context.
    """
    cause: XnGroupCause | XnCause
    context: List[Tuple[str,FileAndLine]]

    def __init__(self, cause: XnGroupCause | XnCause) -> None:
        self.cause = cause
        self.context = []
        pass

    def __str__(self)->str:
        '''programmer friendly format, each context and cause includes
        file and line, and intermediate stack entries are included
        - see unit test below for example'''
        result = ''
        x = ''.join([
            '{fl}failed to {s} because\n'.format(**vars())
            for s,fl in reversed(self.context)])
        return f"{x}{self.cause}"

    def phrase_repr(self)->str:
        '''human (non-programmer) readable representation, omitting file
        and line, omitting intermediate stack entries, and producing a
        proper sentence i.e. capitalised and ending in full stop
        - see unit test below for example'''
        result = ''
        x:str = ''.join([
            'failed to {s} because\n'.format(**vars())
            for s,fl in reversed(self.context)
            if fl.readable])
        cause = self.cause.phrase_repr()
        return f"{x}{cause}"
    pass


def capitalise(s:str)->str:
    if s and s[0]!=s[0].upper():
        return s[0].upper()+s[1:]
    return s

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

def indent(prefix:str,s:str)->str:
    '''prefix all but first line of s by specified prefix'''
    return s.replace('\n','\n'+prefix)

class AllFailed(ExceptionGroup):
    """deprecated now that python has ExceptionGroup"""
    @overload
    def __new__(cls,msg:str,causes:Sequence[Exception]):
        ...
    @overload
    def __new__(cls,causes:Sequence[Exception]):
        ...
    def __new__(cls,*args):
        if type(args[0]) is str:
            return ExceptionGroup.__new__(AllFailed, args[0], args[1])
        return ExceptionGroup.__new__(AllFailed, '', args[0])
    def __str__(self) -> str:
        if self.message:
            return f"{self.message}:\n  {'\nAND\n'.join('  '+indent('  ',str(cause)) for cause in self.exceptions)}"
        return ', and\n'.join('- '+indent('  ',str(cause)) for cause in self.exceptions)
    pass
