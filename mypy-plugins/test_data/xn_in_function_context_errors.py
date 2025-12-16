from xju.xn import in_function_context


def b(x: int) -> int:
    'bb {yy} {x}'
    try:
        return 1
    except Exception:
        raise in_function_context(fred, vars())  # fred not found

def a(x: int) -> int:
    try:
        return x
    except Exception:
        raise in_function_context(a, vars())  # missing doc string

def c(x: list[int]) -> BaseException:
    'do something with {x}, {i}, {p} and {d}'
    for i in x:
        p=2
        if i:
            continue
        d=5
    return in_function_context(c, vars()) # i, p and d not valid

def cc(x: int):
    class C:
        def f(self):
            "{x} {y}"
            try:
                print(x)
            except Exception:
                raise in_function_context(C.f, vars()) from None  # y not valid
            pass
        def q(self):
            "{x} {y}"
            try:
                print('f')
            except Exception:
                raise in_function_context(C.f, globals()) from None  # 2nd param should be vars()
            pass
        pass
    pass

class E1(Exception): pass
class E2(Exception): pass

def d(x: int) -> None:
    '''do something with {x} and {e}'''
    try:
        e:Exception
        try:
            b=3
            raise Exception('fred')
        except E2 as e2:
            e=e2
        except E1 as e1:
            e=e1
        else:
            e=E1('fake e1')
        pass
    except E1:
        raise in_function_context(d, vars()) # e not valid (actually ok but plugin does not analyze exception types raised by inner blocks and even if it did, it would assume b=3 or Exception('fred') could raise E1 so would still give an error)
    pass

def f():
    'use {C}'
    class C: pass
    raise in_function_context(f, vars())  # no error
    pass

def gg(b: bool, d: bool, p: int, pp: int):
    "{j} {q}"
    if p:
        raise Exception('fred')
    elif pp:
        j='jock'
    else:
        j='and'
        pass
    try:
        e=1
    except Exception:
        raise in_function_context(gg, vars()) from None  # q not valid

def g(b: bool, d: bool, p: int, pp: int, m: bool):
    "{a} {b} {c} {d} {e} {f} {j} {t} {m} {v} {u}"
    a=1
    c:int|str
    if b:
        c=2
    else:
        c='fred'
        pass
    if d:
        if p:
            raise Exception('fred')
        elif pp:
            qq=1
            j='jock'
        else:
            j='and'
            pass
        pass
    else:
        j='not'
        pass
    try:
        e=1
        u=p-1
        if u:
            v:int|str
            if p:
                v=2
                raise Exception('fred')
            else:
                v='jock'
    except Exception:
        aa=7
        if m:
            raise Exception('fred')
        else:
            t=8
            raise in_function_context(g, vars()) from None  # e, f, v, u not valid
        pass
    pass

def k(kk:int):
    "{a} {b}"
    try:
        p=2*kk
    except Exception:
        if p:
            a='nine'
            raise in_function_context(k, vars())  # b not valid
        else:
            raise in_function_context(k, vars())  # a, b not valid
        pass
    pass

def ll(x:str, y:int, z:str, o:list):
    "{x:02d} {y:d} {z} {o:d}"
    try:
        q=1
        raise Exception('goat')
    except Exception:
        raise in_function_context(ll, vars())  # x wrong type
    pass

def mm(y: str):
    "mm {y}"
    try:
        raise Exception('mm')
    except Exception:
        raise in_function_context(mm)  # wrong number of params
    pass

async def nnf() -> None:
    pass

async def nn(u: int):
    "nn {u:d}"
    try:
        match u:
            case 1:
                await nnf()
                p=2
            case 2:
                pass
        pass
    except Exception:
        raise in_function_context(vars(), nn)  # wrong param order
    pass

from abc import abstractmethod

class o:
    @abstractmethod
    def ff(self, s: str) -> int:
        '''transform {s} to int'''
        pass
    pass

from typing import overload

class oo(o):
    def ff(self, s: str) -> int:
        try:
            return int(s)
        except Exception:
            raise in_function_context(o.ff,vars())
        pass
    @overload
    def gg(self, s: str) -> int:
        'convert {s} to in'
        ...
    @overload
    def gg(self, i: int) -> str:
        'convert {i} to str'
        ...
    def gg(self, x):
        'convert int/str {g} to str/int'
        try:
            match x:
                case int():
                    return str(x)
                case str():
                    return int(x)
        except Exception:
            raise in_function_context(oo.gg,vars())  # no g
        pass

    @property
    def p(self) -> int:
        'get {self.p}'
        try:
            return 2
        except Exception:
            raise in_function_context(oo.p,vars())  # property+setter not supported as ambiguous
        pass
    @p.setter
    def p(self, i: int) -> None:
        'set {self}.p to {x}'
        try:
            pass
        except Exception:
            raise in_function_context(oo.p,vars())  # property+setter not supported as ambiguous
        pass
    @overload  # missing implementation
    def qq(self, s: str) -> int:
        'convert {s} to in'
        ...
    pass

from functools import wraps

def my_decorator(f):
    @wraps(f)
    def wrapper(*args, **kwds):
        '''call {f}() with args {args} and kwargs {kwargs}'''
        try:
            return f(*args, **kwds)
        except Exception:
            raise in_function_context(wrapper, vars()) from None  # wraps loses doc string
        pass
    return wrapper

from contextlib import contextmanager
@contextmanager
def cm():
    '''run in context'''
    try:
        yield
    except:
        raise in_function_context(cm, vars())
    pass

class po:
    @property
    def p(self) -> int:
        'get {self.p}'
        try:
            return 2
        except Exception:
            raise in_function_context(po.p,vars())  # good
        pass
