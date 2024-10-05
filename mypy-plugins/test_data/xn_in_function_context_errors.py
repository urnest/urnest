from xju.xn import in_function_context

def b(x: int) -> int:
    'bb {yy} {x}'
    try:
        return 1
    except Exception:
        raise in_function_context(fred, vars())

def a(x: int) -> int:
    try:
        return x
    except Exception:
        raise in_function_context(a, vars())
