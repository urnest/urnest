from xju.xn import in_function_context

import m2

class BI(m2.B):
    def ff(self, s: str) -> int:
        try:
            return int(s)
        except Exception:
            raise in_function_context(m2.B.ff,vars())
        pass
    
