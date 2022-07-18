import sys
import os

def test1():
    print('starting some testing',file=sys.stderr)
    print('output of run')
    assert os.environ.get('TEST_PORT') == '9999'
    assert True
