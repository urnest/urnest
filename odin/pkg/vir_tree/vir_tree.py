#!/bin/env python

import sys
import traceback
import os
import string

class E:
    def __init__(self, s):
        self.s=s
    def __str__(self):
        return self.s
    pass

def main(ODIN_spec, ODIN_select):
    spec=file(ODIN_spec).read().splitlines()
    select=''
    if ODIN_select:
        select=ODIN_select+'/'
    selected=[x for x in spec if len(x.split()) and x.startswith(select)]
    parsed=[_[len(select):].split('==', 1) for _ in selected]
    malformed=['line %s'%(_[0]+1) for _ in zip(range(0, len(parsed)),
                                             parsed) if len(_[1])!=2]
    if len(malformed):
        raise E("the following lines have no '==': " +
                string.join(malformed, ', '))

    parsed=[(_[0].strip().split('/'), _[1].strip()) for _ in parsed]
    subdirs=set([_[0][0] for _ in parsed if len(_[0])>1])
    files=[(_[0][0], _[1]) for _ in parsed if len(_[0])==1]
    lines=[r'%'+'%s==%s\n' % _ for _ in files]+\
          [r'%'+"%s==%s+select='%s':vir_tree\n"%(_, ODIN_spec, select+_) for _ in subdirs]
    result=string.join(lines, '')
    return result
    
if __name__=='__main__':
    if os.environ.has_key('ODINVERBOSE'):
        print string.join(sys.argv, ' ')

    ODIN_spec, ODIN_select=sys.argv[1:3]
    
    sys.stdout=file('vir_dir_spec', 'w')
    try:
        print main(ODIN_spec, ODIN_select)
    except:
        traceback.print_exc(file=file("ERRORS", 'w'))
    pass
