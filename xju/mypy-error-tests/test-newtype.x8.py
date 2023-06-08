from typing import NewType
Mango = NewType('Mango',str)
Kiwi = NewType('Kiwi',str)
a=Mango('green')
b=Kiwi('gree')
print(a==b)
