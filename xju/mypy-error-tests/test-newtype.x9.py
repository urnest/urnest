from xju.newtype import Str
class AppleTag:pass
class OrangeTag:pass
class Apple(Str[AppleTag]):pass
class Orange(Str[OrangeTag]): pass
x:Apple=Apple('red')
y:Orange=Orange('red')
print(x==y)
