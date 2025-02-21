from abc import abstractmethod
from xju.xn import in_function_context

class B:
    @abstractmethod
    def ff(self, s: str) -> int:
        '''transform {s} to int'''
        pass
    pass


class BI(B):
    def ff(self, s: str) -> int:
        try:
            return int(s)
        except Exception:
            raise in_function_context(B.ff,vars())
        pass
