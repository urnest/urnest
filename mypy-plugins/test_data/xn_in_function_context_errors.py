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
