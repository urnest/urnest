#!/bin/env python3

import os
import sys

ODIN_FILE,ODIN_DIR,ODIN_incsp,ODIN_implsp,ODIN_ignore,ODIN_suffixes=sys.argv[1:]

if os.environ.get('ODINVERBOSE',False):
    def log(x,h=os.environ.get('ODINRBSHOST','')):
        print('{h}{x}')
        pass
else:
    def log(x):
        pass
    pass

def error(x):
    with open('ERRORS','w+') as f:
        f.write(x+'\n')
        pass
    pass

ignore=open(ODIN_ignore).read().split() if ODIN_ignore else []
suffixes=open(ODIN_suffixes).read().split() if ODIN_suffixes else []
if not len(suffixes):
    suffixes=os.environ.get('ODIN_O_SRC_SUFFIXES').split()
    pass

incsp=open(ODIN_incsp).read().split() if ODIN_incsp else []
implsp=open(ODIN_implsp).read().split() if ODIN_implsp else []
odincxxi=os.environ.get('ODIN_CXX_I','').split()

relinc=[i for i in incsp if not i.startswith('/')]
relimp=[i for i in implsp if not i.startswith('/')]
relcxxi=[i for i in odincxxi if not i.startswith('/')]
if len(relinc):
    error('+inc_sp {relinc!r} must all be absolute')
elif len(relimp):
    error('+impl_sp {relimp!r} must all be absolute')
elif len(relcxxi):
    error('ODIN_CXX_I entries {relcxxi!r} must be absolute')
else:
    dirs=(implsp or incsp)+odincxxi
    log('scan_for_impls {ODIN_FILE} in {ODIN_DIR} and {dirs}'.format(**vars()))
    localdirs=[ODIN_DIR]+dirs
    r=re.compile('[ \t]*#[ 	]*include[ \t]*([<"])([^>"]*).*')
    includes=[]
    for line in open(ODIN_FILE).readlines():
        m=r.match(line)
        if m:
            if m.groups()[0]=='"':
                includes.append(m.groups()[1],localdirs)
            else:
                includes.append(m.groups()[1],dirs)
                pass
            pass
        pass
    with open('hash_impl.view_desc','w') as f:
        for name,sp in includes:
            prefix='.'.join(name.split('.')[0:-1]) if '.' in name else name
            if name.startswith('/'):
                f.write("'{name}'\n".format(**vars()))
                for suffix in suffixes:
                    f.write("'{prefix}{suffix}'\n".format(**vars()))
                    pass
                f.write("=''\n")
                pass
            else:
                for dir in sp:
                    f.write("'{dir}/{name}'\n".format(**vars()))
                    f.write("=''\n")
                    pass
                for dir in sp:
                    for suffix in suffixes:
                        f.write("'{dir}/{prefix}{suffix}'\n'".format(**vars()))
                        pass
                    f.write("=''\n")
                    pass
                pass
            pass
        pass
    pass
            
    
