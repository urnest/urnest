from xju.xn import in_function_context

def b(x: int) -> int:
    'bb {yy} {x}'
    try:
        return 1
    except Exception:
        raise in_function_context(fred, vars())  # yy not found

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
        raise in_function_context(d, vars()) # actually ok but plugin does not analyze exception types raised by inner blocks and even if it did, it would assume b=3 or Exception('fred') could raise E1 so would still give an error
    pass

def f():
    'use {C}'
    class C: pass
    raise in_function_context(f, vars())  # no error
    pass

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
            raise in_function_context(g, vars()) from None  # REVISIT: should not complain about j but does

def k(kk:int):
    "{a} {b}"
    try:
        p=2*kk
    except Exception:
        if p:
            a='nine'
            raise in_function_context(k, vars())
        else:
            raise in_function_context(k, vars())


def c(x: int):
    class C:
        def f(self):
            "{x} {y}"
            try:
                print(x)
            except Exception:
                raise in_function_context(C.f, vars()) from None
            pass
        def q(self):
            "{x} {y}"
            try:
                print('f')
            except Exception:
                raise in_function_context(C.f, globals()) from None  # REVISIT: should be no complaint about x but there is
            pass
        pass
    pass
