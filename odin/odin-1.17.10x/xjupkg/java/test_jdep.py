#!/bin/env python

import jdep
import string

def test1():
    x='''\
1.java:5: class k is public, should be declared in a file named k.java
public class k {
       ^
'''
    y, r=jdep.parseErrorBlock(x.splitlines(True), 1)
    assert len(r)==0, (y, r)
    assert y.fileName=='1.java', y
    assert y.lineNumber==5, y
    assert ''.join(y.lines)==x, y.lines


def test2():
    x='''\
1.java:5: class k is public, should be declared in a file named k.java
public class k {
       ^
1.java:3: package d does not exist
import d.*;
^
'''
    y, r=jdep.parseErrorBlock(x.splitlines(True), 1)
    assert len(r)==3, (y, r)
    assert y.fileName=='1.java', y
    assert y.lineNumber==5, y
    assert ''.join(y.lines+r,)==x

def test3():
    x='''\
1.java:5: class k is public, should be declared in a file named k.java
public class k {
       ^
1.java:3: package d does not exist
import d.*;
^
4 errors
'''
    y=jdep.parseErrorBlocks(x.splitlines(True))
    assert len(y)==2, [str(_) for _ in y]

    assert y[0].fileName=='1.java', y[0]
    assert y[0].lineNumber==5, y[0]
    assert y[1].fileName=='1.java', y[1]
    assert y[1].lineNumber==3, y[1]
   
    assert ''.join(y[0].lines+y[1].lines)==x, y.lines
    
    
if __name__=='__main__':
    n=len([globals()[x]() for x in globals().keys() if x.startswith('test')])
    print('PASS - %(n)s steps' % vars())
    pass
