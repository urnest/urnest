#!/usr/bin/env python3

import os
import os.path
import string
import sys

def main(argv):
    ODIN_file=sys.argv[1]
    assert os.path.isdir(ODIN_file), '%(ODIN_file)s is not a directory'%vars()
    x=[os.path.join(ODIN_file, _) for _ in os.listdir(ODIN_file)]
    y=[_ for _ in x if os.path.isdir(_)]
    return [ODIN_file.replace('+','\\+')+'\n']+[_.replace('+','\\+')+':dirtree\n' for _ in y]

if __name__=='__main__':
    open('dirtree', 'w').write(''.join(main(sys.argv)))
    pass
