#!/usr/bin/env python

import os
import sys
import re
import os.path

odinrbshost=os.environ.get('ODINRBSHOST','')

py,dir_,pySp,ignore=sys.argv[1:]

err=file('ERROR','w')
viewDesc=file('py_import.view_desc','w')

class Scope:
    def __init__(self,desc):
        self.desc=desc
        pass
    def __enter__(self):
        if os.environ.get('ODINVERBOSE',''):
            print '{'+'{desc}'.format(**self.__dict__)
            pass
        pass
    def __exit__(self,a,b,c):
        if os.environ.get('ODINVERBOSE'):
            print '}'+'{desc}'.format(**self.__dict__)
            pass
        pass
    pass

with Scope('scan for py imports {py}'.format(**vars())):
    if pySp: pySp=file(pySp).read().split()
    
    rel=' '.join([_ for _ in pySp if not _.startswith('/')])
    if rel:
        print >>err, '+py_sp entries must be absolute: {rel}'.format(**vars())
        sys.exit(0)
        pass

    lines=file(py).read().split('\n')
    modules=[]
    
    # pick up lines that start with "import" eg "import os as a, sys, ..d"
    r1=re.compile('^[ \t]*import[ \t]+(.*)')
    
    # pick up lines that start with "from" eg "from os import c as d, b"
    r2=re.compile('^[ \t]*from[ \t]+([^ \t]*)[ \t]+import[ \t]+(.*)')

    m1s=[r1.match(line) for line in lines]
    m2s=[r2.match(line) for line in lines]

    for i,m in enumerate(m1s):
        if m:
            line=lines[i]
            with Scope('analyse import line {line}'.format(**vars())):
                matched=m.groups()[0]
                n=0
                while i+n<len(lines) and matched.endswith('\\'):
                    matched=matched[0:-1]+' '+lines[i+n]
                    n=n+1
                    pass
                clauses=matched.split(',')
                # each clauses could be like "a", " asb   as  cas "
                modules.extend([' '.join(clause.strip().split()).split(' as ')[0]
                                for clause in clauses])
                pass
            pass
        pass
    
    for i,m in enumerate(m2s):
        if m:
            line=lines[i]
            with Scope('analyse from line {line}'.format(**vars())):
                fromModule=m.groups()[0].strip()
                matched=m.groups()[1]
                n=0
                while i+n<len(lines) and matched.endswith('\\'):
                    matched=matched[0:-1]+' '+lines[i+n]
                    n=n+1
                    pass
                clauses=matched.split(',')
                # each clauses could be like "a", " asb   as  cas "
                modules.extend([fromModule+'.'+
                                ' '.join(clause.strip().split()).split(' as ')[0]
                                for clause in clauses])
                pass
            pass
        pass
    
    ignoreREs=[]
    if ignore:
        rs=file(ignore).read().split('\n')
        for r in [_ for _ in rs if _]:
            with Scope('ignore modules matching +ignore regular expression {r}'.format(
                    **vars())):
                re=re.compile(r)
                ignoreREs.append(re)
                pass
            pass
        pass
    if os.environ.get('ODIN_PY_IGNORE',''):
        with Scope('ignore modules matching \$ODIN_PY_IGNORE regular expression {r}'.format(
                    **vars())):
            re=re.compile(os.environ.get('ODIN_PY_IGNORE',''))
            ignoreREs.append(re)
            pass
        pass
    
    leadingDotsRE=re.compile('^([.]+)(.*)')
    for module in modules:
        m=leadingDotsRE.match(module)
        if m:
            leadingDots=m.groups()[0]
            rest=m.groups()[1]
            rel=['..']*(len(leadingDots)-1)+rest.split('.')
            sp=[dir_]
            pass
        else:
            sp=[dir_]+pySp # e.g. ['/lib','/home/xju/py']
            rel=module.split('.') # e.g. ['os','path']
            pass
        for d in sp:
            f=d
            #when doing multi-level all __init__.py's along the way are read
            #so generate dependencies on them all but only the last can
            #mention x.py; others can only specify x/__init__.py
            for x in rel[0:-1]:
                f=os.path.normpath(os.path.join(f,x))
                if not [_ for _ in ignoreREs if _.match(f)]:
                    viewDesc.write("'"+f+'/__init__.py'+"'\n")
                    viewDesc.write("=''\n")
                    pass
                pass
            f=os.path.normpath(os.path.join(f,rel[-1]))
            if not [_ for _ in ignoreREs if _.match(f)]:
                viewDesc.write("'"+f+".py'\n")
                viewDesc.write("'"+f+"/__init__.py'\n")
                viewDesc.write("=''\n")
                pass
            pass
        pass
    pass
