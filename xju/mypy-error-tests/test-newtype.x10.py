from xju.newtype import Str
class AppleTag:pass
class Apple(Str[AppleTag]):pass
x:Apple=Apple('red')
y:str='red'
print(x==y)  # mypy --strict-equality should give an error but 1.3.0 does not
