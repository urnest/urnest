#!/bin/env python

import re
import os
import string
import sys

to_windows_re='/cygpath/([^/]*)(/.*)'
def to_windows(path, re):
    x=re.match(path)
    return ('%s:%s' % x.groups()).replace('/', '\\')

def main(argv):
    ODIN_classpath=argv[1]
    if not ODIN_classpath:
        return ''
    unix_paths=[os.environ('ODIN_JAVA_OUTPUT_DIRECTORY')]+\
                open(ODIN_classpath).read.split('\n')
    if os.environ.getdefault('ODIN_JAVA_WINDOWS_COMPILER', None):
        r=re.compile(to_windows_re)
        paths=[to_windows(_, r) for _ in paths]
        return ';'.join(paths)
    return ':'.join(unix_paths)
    pass

if __name__=='__main__':
    open('compiler_classpath', 'w').write(main(sys.argv))
    pass
