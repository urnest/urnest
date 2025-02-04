import sys

def test1():
    print('starting some testing',file=sys.stderr)
    print('output of run')
    assert 1 == 2  # type: ignore
