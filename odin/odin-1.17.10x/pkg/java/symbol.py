#!/bin/env python3

import jdep
import traceback
import sys
import os
import string

def main(argv):
    ODIN_java=argv[1]
    ODIN_classnames=argv[2]
    ODIN_syms=argv[5]
    ODIN_syms_file=argv[6]
    package=jdep.getPackageOfFile(ODIN_java)
    s=open(ODIN_classnames).read().splitlines()[0].split()[1]
    if ODIN_java==s:
        assert s.endswith('.java')
        s=os.path.basename(s)[0:-5]
    print(package+'.'+s)
    if len(ODIN_syms):
        print('\n'.join(open(ODIN_syms).read().split()))
    if len(ODIN_syms_file):
        print('\n'.join(open(ODIN_syms_file).read().split()))
        
if __name__=='__main__':
    try:
        if os.environ.has_key('ODINVERBOSE'):
            print(' '.join(sys.argv))
        sys.stdout=open('symbol', 'w')
        main(sys.argv)
    except:
        traceback.print_exc(file=open("ERRORS", 'w'))
    pass
