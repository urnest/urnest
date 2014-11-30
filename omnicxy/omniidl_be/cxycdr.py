from omniidl import idlast
from omniidl import idltype

import sys
import os.path

interface_t='''\
template<>
class cdr< ::%(fqn)s>
{
public:
  static const char repoId[]="%(repoId)s";
};
'''

def gen(decl,eclass,eheader,indent=''):
    result=''
    if isinstance(decl, idlast.Module):
        result=''.join(gen(_,eclass,eheader) for _ in decl.definitions())
    elif isinstance(decl, idlast.Interface):
        fqn='::'.join(decl.scopedName())
        repoId=decl.repoId()
        result=interface_t%vars()
    elif isinstance(decl, idlast.Operation):
        pass
    elif isinstance(decl, idlast.Typedef):
        pass
    else:
        assert False, repr(decl)
        pass
    return result

template='''\
// generated from %(fileName)s by omnicxy cxycdr idl backend

#include <cxy/cdr.hh>

#include "%(baseName)s.hh"
%(idlincludes)s

namespace cxy
{
%(items)s
}
'''

def includeSpec(fileName):
    if os.path.dirname(fileName)=='':
        return '"%s"'%(os.path.splitext(fileName)[0]+'.cdr.hh')
    return '<%s>'%(os.path.splitext(fileName)[0]+'.cdr.hh')

def gen_idlincludes(fileNames):
    if not len(fileNames):
        return ''
    return '\n// included idl'+''.join(['\n#include %s'%includeSpec(_) for _ in fileNames])

def run(tree, args):
    eclass,eheader=([_.split('-e',1)[1].split('=',1) for _ in args if _.startswith('-e')]+[('cxy::Exception','cxy/Exception.hh')])[0]
    if eheader.startswith('./') or os.path.dirname(eheader)=='':
        eheader='"%s"'%eheader[2:]
    else:
        eheader='<%s>'%eheader
    assert tree.file().endswith('.idl'), tree.file()
    fileName=os.path.basename(tree.file())
    baseName=fileName[0:-4]
    items=''.join([gen(_,eclass,eheader) for _ in tree.declarations() if _.mainFile()])
    idlincludes=gen_idlincludes(set([_.file() for _ in tree.declarations() if not _.mainFile()]))
    print template % vars()
    pass
