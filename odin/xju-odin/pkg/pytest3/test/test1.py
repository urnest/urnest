import sys
import os
import m1

def test1():
    print('starting some testing',file=sys.stderr)
    print('output of run')
    assert os.environ.get('TEST_PORT') == '9999'
    assert True

def test2():
    assert m1.x() == 1
