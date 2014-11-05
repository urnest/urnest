from omniidl import idlast
from omniidl import idltype

import sys
import os.path

interface_t='''\
class %(name)s
{
public:
  virtual ~F() throw()
  {
  }
%(content)s
};
'''

operation_t='''\
virtual void %(name)s() throw(
  // ipc failure
  // - note servant may not throw
  %(eclass)s) = 0;'''

def reindent(indent, s):
    '''prepend %(indent)r to each line of %(s)r'''
    return '\n'.join([indent+_ for _ in s.split('\n')])

def gen(decl,eclass,eheader,indent=''):
    result=''
    if isinstance(decl, idlast.Module):
        ns=decl.identifier()
        result='%(indent)snamespace %(ns)s\n%(indent)s{\n'%vars()
        result=result+''.join([gen(_,eclass,eheader,indent)+'\n' for _ in decl.definitions()])
        result=result+'%(indent)s}'%vars()
    elif isinstance(decl, idlast.Interface):
        name=decl.identifier()
        content='\n'.join([gen(_,eclass,eheader,indent+'  ') for _ in decl.contents()])
        result=reindent(indent,interface_t%vars())
    elif isinstance(decl, idlast.Operation):
        name=decl.identifier()
        assert not decl.oneway(), 'oneway not yet implemented'
        assert len(decl.parameters())==0, 'parameters not yet implemented'
        assert len(decl.raises())==0, 'raises not yet implemented'
        assert len(decl.contexts())==0, 'contexts not yet implemented'
        assert isinstance(decl.returnType(),idltype.Base) and decl.returnType().kind()==idltype.tk_void, 'returns not yet implemented'
        result=reindent(indent,operation_t%vars())
    else:
        assert False, repr(decl)
        pass
    return result

head='''\
// generated from %(fileName)s by omni cxy idl backend specifying 
// %(eclass)s from %(eheader)s as base class for all ipc exceptions

#include %(eheader)s
'''

def run(tree, args):
    eclass,eheader=([_.split('-e',1)[1].split('=',1) for _ in args if _.startswith('-e')]+[('cxy::Exception','cxy/Exception.hh')])[0]
    if eheader.startswith('./') or os.path.dirname(eheader)=='':
        eheader='"%s"'%eheader[2:]
    else:
        eheader='<%s>'%eheader
    fileName=os.path.basename(tree.file())
    print head % vars()
    print '\n'.join([gen(_,eclass,eheader) for _ in tree.declarations() if _.mainFile()])
    pass
