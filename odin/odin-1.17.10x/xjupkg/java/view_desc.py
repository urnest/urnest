#!/bin/env python

import sys
import traceback
import os
import string

def printViewDesc(required_symbols, inventories, sp):
    for s in required_symbols:
        sf=s.replace('.', '/')
        for inv in inventories:
            print "'%(inv)s/%(s)s'" % vars()
            pass
        for c in sp:
            print "'%(c)s/%(sf)s.java'" % vars()
            pass
        print "= ''"
        pass
    pass
pass

def main(argv):
    ODIN_symbols, ODIN_inventories, ODIN_sp=argv[1:4]

    symbols=file(ODIN_symbols).read().splitlines()
    inventories=[]
    if ODIN_inventories: inventories=file(ODIN_inventories).read().splitlines()
    sp=[]
    if ODIN_sp: sp=file(ODIN_sp).read().splitlines()

    printViewDesc(symbols, inventories, sp)
    
if __name__=='__main__':
    try:
        if os.environ.has_key('ODINVERBOSE'):
            print string.join(sys.argv)
        sys.stdout=file('java.view_desc', 'w')
        main(sys.argv)
    except:
        traceback.print_exc(file=file("ERRORS", 'w'))
        sys.exit(1)
    pass
