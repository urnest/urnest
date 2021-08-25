#!/usr/bin/env python

import os
import sys
import re
import os.path

odinrbshost=os.environ.get('ODINRBSHOST','')

py,dirOfPy,pyMainDir,pySp,ignore=sys.argv[1:]

err=open('ERRORS','w')
viewDesc=open('py_import.view_desc','w')

indent=''

class Scope:
    def __init__(self,desc,on=True):
        self.desc=desc
        self.result_=None
        self.on=on
        pass
    def __enter__(self):
        if os.environ.get('ODINVERBOSE','') and self.on:
            global indent
            print(indent+'{desc}'.format(**self.__dict__))
            indent=indent+'  '
            pass
        return self
    def __exit__(self,exceptionType,exception,traceBack):
        if os.environ.get('ODINVERBOSE') and self.on:
            global indent
            if exception:
                print(indent+'*** failed')
            elif self.result_:
                print(indent+'-> {result_}'.format(**self.__dict__))
            indent=indent[0:-2]
            pass
        pass
    def result(self,x):
        self.result_=x
    pass

with Scope('scan for py imports {py} noting main dir {pyMainDir}'.format(**vars())):
    if not pyMainDir:
        print(f'+py_r must be specified e.g. via {py}:py_rd rather than just {py}',file=err)
        sys.exit(0)
        pass
    pySp=open(pySp).read().split() if pySp else []
    eggs=[_ for _ in pySp if _.endswith('.egg')]
    pySp=[_ for _ in pySp if not _.endswith('.egg')]

    rel=' '.join([_ for _ in pySp if not _.startswith('/')])
    if rel:
        print('+py_sp entries must be absolute: {rel}'.format(**vars()),file=err)
        sys.exit(0)
        pass

    lines=open(py).read().split('\n')
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
            with Scope('analyse py import line {line}'.format(**vars())):
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
        rs=open(ignore).read().split('\n')
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
    allModules=set()
    with Scope('expand modules {modules}'.format(**vars()),False) as scope:
        for module in modules:
            m=leadingDotsRE.match(module)
            if m:
                leadingDots=m.groups()[0]
                rest=m.groups()[1]
                rel=['.']+['..']*(len(leadingDots)-1)+rest.split('.') #e.g. ['..','a','b']
                pass
            else:
                rel=module.split('.') # e.g. ['os','path']
                pass
            for i in range(0,len(rel)):
                allModules.add(tuple(rel[0:i+1]))
                pass
            pass
        scope.result(allModules)
        pass
    for module in allModules:
        with Scope('generate {module} viewdesc'.format(**vars()),False):
            if module[0]=='.':
                sp=[dirOfPy]
            else:
                sp=[pyMainDir]+pySp # e.g. ['/lib','/home/xju/py']
                pass
            for d in sp:
                f=os.path.normpath(os.path.join(d,*module))
                viewDesc.write("'{f}.py'\n".format(**vars()))
                # import from file's or ancestor's module does not depend on
                # that module's __init__.py
                #  e.g. f might be /a/b/c
                # and our file might be /a/b/c/d/q.py so dirOfPy /a/b/c/d
                if os.path.split(dirOfPy)[0:len(os.path.split(f))]!=os.path.split(f):
                    viewDesc.write("'{f}/__init__.py'\n".format(**vars()))
                    pass
                pass
            viewDesc.write("=''\n")
            pass
        pass
    for e in eggs:
        viewDesc.write("'"+e+"'\n")
        viewDesc.write("='"+e+"'\n")
        pass
    pass
