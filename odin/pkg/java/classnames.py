#!/bin/env python

import re
import string
import sys
import os

re_class_name=re.compile('(.*[.]java):[0-9][0-9]*: class ([^ ]*) is public, should be declared in a file named .*')

def getClassNames(files_filename, raw_errors_filename):
    filenames=file(files_filename).read().split()
    filenumbers=range(1, len(filenames)+1)
    f=zip(filenames, filenumbers)
    matches=[re_class_name.match(l) for l in \
             file(raw_errors_filename).readlines()]
    m=dict([_.groups()[0:2] for _ in matches if not _ is None])
    result=[_[0] + ' ' + m.get(str(_[1])+'.java', _[0])+'\n' for _ in f]
    return string.join(result, '')

if __name__=='__main__':
    if os.environ.has_key('ODINVERBOSE'):
        print string.join(sys.argv)
    file(sys.argv[3], 'w').write(getClassNames(sys.argv[1], sys.argv[2]))
    
