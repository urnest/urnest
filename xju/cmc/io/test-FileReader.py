#!/usr/bin/env python3

from xju.cmc.io import FileReader
from pathlib import Path
from xju.assert_ import Assert
from xju.xn import readableRepr

try:
    with FileReader(Path("xxx.txt")) as f:
        pass
except Exception as e:
    Assert(readableRepr(e))=="Failed to open xxx.txt reader with close-on-exec True because\n[Errno 2] No such file or directory: 'xxx.txt'."
else:
    assert False, f'should not be here with {f}'
    pass

with open('xxx.txt','w') as f2:
    f2.write('fred')
    pass

with FileReader(Path("xxx.txt")) as f:
    Assert(f.readall().decode('utf-8'))=="fred"
    pass
