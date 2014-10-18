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

def gen(decl,indent=''):
    result=''
    if isinstance(decl, idlast.Module):
        result=''.join(gen(_) for _ in decl.definitions())
    elif isinstance(decl, idlast.Interface):
        fqn='::'.join(decl.scopedName())
        repoId=decl.repoId()
        result=interface_t%vars()
    elif isinstance(decl, idlast.Operation):
        pass
    else:
        assert False, repr(decl)
        pass
    return result

template='''\
// generated from %(fileName)s by cxy.misc idl backend

#include <cxy/cdr.hh>

#include "%(baseName)s.hh"

namespace cxy
{
%(items)s
}
'''

def run(tree, args):
    assert tree.file().endswith('.idl'), tree.file()
    fileName=os.path.basename(tree.file())
    baseName=fileName[0:-4]
    items=''.join([gen(_) for _ in tree.declarations() if _.mainFile()])
    print template % vars()
    pass
