from xju.xn import in_function_context

def a(x: int) -> int:
    try:
        return x
    except Exception:
        raise in_function_context(a, vars())

def b(x: int) -> int:
    'bb {xx}'
    try:
        return x
    except Exception:
        raise in_function_context(b, vars())
