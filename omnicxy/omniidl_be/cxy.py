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
virtual void %(name)s(%(params)s) throw(
  // ipc failure
  // - note servant may not throw
  %(eclass)s) = 0;'''

class TypeInfo:
    def __init__(self, typename, includeFiles):
        self.typename=typename
        self.includeFiles=includeFiles
        pass
    pass

basicParamTypes={
    idltype.tk_short:  TypeInfo('int16_t',['<stdint.h>']),
    idltype.tk_long:   TypeInfo('int32_t',['<stdint.h>']),
    idltype.tk_double: TypeInfo('double',[]),
    idltype.tk_string: TypeInfo('std::string',['<string>'])
}
def unqualifiedType(p):
    assert isinstance(p,idlast.Parameter),p
    assert p.paramType().kind() in basicParamTypes, '%s not implemented, only basic types %s implemented' % (p.paramType().kind(),basicParamTypes.keys())
    return basicParamTypes.get(p.paramType().kind()).typename
    
def ptype(p):
    assert isinstance(p,idlast.Parameter),p
    if p.dirtext()=='in':
        assert p.paramType().kind() in basicParamTypes, '%s not implemented, only basic types %s implemented' % (p.paramType().kind(),basicParamTypes.keys())
        return basicParamTypes.get(p.paramType().kind()).typename+' const'
    assert p.dirtext()+' params not yet implemented'

def tincludes(p):
    assert p.paramType().kind() in basicParamTypes, '%s not implemented, only basic types %s implemented' % (p.paramType().kind(),basicParamTypes.keys())
    return basicParamTypes.get(p.paramType().kind()).includeFiles
    
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
        params=','.join(['\n  %s& %s'%(ptype(p),p.identifier()) for p in decl.parameters()])
        assert not decl.oneway(), 'oneway not yet implemented'
        assert len(decl.raises())==0, 'raises not yet implemented'
        assert len(decl.contexts())==0, 'contexts not yet implemented'
        assert isinstance(decl.returnType(),idltype.Base) and decl.returnType().kind()==idltype.tk_void, 'returns not yet implemented'
        result=reindent(indent,operation_t%vars())
    else:
        assert False, repr(decl)
        pass
    return result

def gen_tincludes(decl):
    result=[]
    if isinstance(decl, idlast.Module):
        result=result+sum([gen_tincludes(_) for _ in decl.definitions()],[])
    elif isinstance(decl, idlast.Interface):
        result=result+sum([gen_tincludes(_) for _ in decl.contents()],[])
    elif isinstance(decl, idlast.Operation):
        result=result+sum([tincludes(p) for p in decl.parameters()],[])
    else:
        assert False, repr(decl)
        pass
    return result

head='''\
// generated from %(fileName)s by omni cxy idl backend specifying 
// %(eclass)s from %(eheader)s as base class for all ipc exceptions

#include %(eheader)s
%(tincludes)s
%(idlincludes)s
'''

def includeSpec(fileName):
    if os.path.dirname(fileName)=='':
        return '"%s"'%(os.path.splitext(fileName)[0]+'.hh')
    return '<%s>'%(os.path.splitext(fileName)[0]+'.hh')

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
    fileName=os.path.basename(tree.file())
    tincludes='\n'.join(['#include '+_ for _ in set(sum([gen_tincludes(_) for _ in tree.declarations() if _.mainFile()],[]))])
    idlincludes=gen_idlincludes(set([_.file() for _ in tree.declarations() if not _.mainFile()]))
    
    print head % vars()
    print '\n'.join([gen(_,eclass,eheader) for _ in tree.declarations() if _.mainFile()])
    pass
