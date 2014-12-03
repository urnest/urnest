from omniidl import idlast
from omniidl import idltype

import sys
import os.path
import types

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
virtual %(returnType)s %(name)s(%(params)s) throw(
  // ipc failure
  // - note servant may not throw
  %(eclass)s) = 0;'''

class TypeInfo:
    def __init__(self, typename, includeFiles):
        self.typename=typename
        self.includeFiles=includeFiles
        pass
    pass

basicIntTypes={
    idltype.tk_short:   TypeInfo('int16_t',['<stdint.h>']),
    idltype.tk_long:    TypeInfo('int32_t',['<stdint.h>']),
    idltype.tk_ushort:  TypeInfo('uint16_t',['<stdint.h>']),
    idltype.tk_ulong:   TypeInfo('uint32_t',['<stdint.h>']),
}
basicFloatTypes={
    idltype.tk_double: TypeInfo('double',[]),
}
basicStringTypes={
    idltype.tk_string: TypeInfo('std::string',['<string>']),
}
basicParamTypes=dict(
    basicIntTypes.items()+
    basicFloatTypes.items()+
    basicStringTypes.items()+{
    idltype.tk_void:   TypeInfo('void',[])
    }.items()
)
def unqualifiedType(t):
    assert isinstance(t,idltype.Type),t
    if t.kind() in basicParamTypes:
        return basicParamTypes.get(t.kind()).typename
    elif t.kind()==idltype.tk_alias:
        return ''.join(['::'+_ for _ in t.scopedName()])
    elif t.kind()==idltype.tk_struct:
        return ''.join(['::'+_ for _ in t.scopedName()])
    elif t.kind()==idltype.tk_sequence:
        assert t.bound()==0, 'bounded sequences not yet implemented'
        itemType=unqualifiedType(t.seqType())
        return 'std::vector< %(itemType)s>'%vars()
    assert False, '%s not implemented, only basic types %s implemented' % (t.kind(),basicParamTypes.keys())
    pass

def ptype(p):
    assert isinstance(p,idlast.Parameter),p
    if p.dirtext()=='in':
        if p.paramType().kind() in basicParamTypes:
            return basicParamTypes.get(p.paramType().kind()).typename+' const'
        elif p.paramType().kind()==idltype.tk_alias:
            return ''.join(['::'+_ for _ in p.paramType().scopedName()])+' const'
        elif p.paramType().kind()==idltype.tk_struct:
            return ''.join(['::'+_ for _ in p.paramType().scopedName()])+' const'
        assert False, '%s not implemented, only basic types %s implemented' % (p.paramType().kind(),basicParamTypes.keys())

    assert p.dirtext()+' params not yet implemented'

def tn(t):
    if type(t) is types.InstanceType:
        return t.__class__
    return type(t)
    
def tincludes(t):
    if t.kind() in basicParamTypes:
        return basicParamTypes.get(t.kind()).includeFiles
    elif t.kind() in [idltype.tk_alias]:
        return []
    elif t.kind() in [idltype.tk_struct]:
        return []
    elif t.kind() in [idltype.tk_sequence]:
        return tincludes(t.seqType())
    assert False, '%s not implemented, only basic types %s implemented' % (t.kind(),basicParamTypes.keys())

def reindent(indent, s):
    '''prepend %(indent)r to each line of %(s)r'''
    return '\n'.join([indent+_ for _ in s.split('\n')])

struct_t='''\
struct %(name)s
{
  %(name)s(%(consparams)s) throw():%(consinitialisers)s {
  }
  %(members)s

};
bool operator<(
    %(name)s const& x, 
    %(name)s const& y) throw() {%(lessMembers)s
    return false;
  }
bool operator>(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return y<x;
  }
bool operator!=(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return (x<y)||(y<x);
  }
bool operator==(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return !(x!=y);
  }
bool operator<=(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return (x<y)||(x==y);
  }
bool operator>=(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return (x>y)||(x==y);
  }
'''


def gen_struct(name,memberTypesAndNames):
    assert len(memberTypesAndNames)>0, name
    memberNames=[_[1] for _ in memberTypesAndNames]
    memberTypes=[_[0] for _ in memberTypesAndNames]
    paramNames=['p%s'%i for i in range(1,len(memberTypesAndNames)+1)]
    
    members=''.join(['\n  %s %s;'%_ for _ in memberTypesAndNames])
    consparams=','.join(['\n    %s const& %s'%_ for _ in zip(memberTypes,paramNames)])
    consinitialisers=','.join(['\n      %s(%s)'%_ for _ in zip(memberNames,paramNames)])
    lessMembers=''.join([('\n    if (x.%(_)s<y.%(_)s) return true;'+
                          '\n    if (y.%(_)s<x.%(_)s) return false;')%vars()\
                             for _ in memberNames])
    return struct_t%vars()

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
        returnType=unqualifiedType(decl.returnType())
        result=reindent(indent,operation_t%vars())
    elif isinstance(decl, idlast.Typedef):
        name=decl.declarators()[0].identifier()
        tagClass=''
        aliasOf=decl.aliasType()
        if aliasOf.kind() in basicIntTypes:
            aliasOf=unqualifiedType(aliasOf)
            result=reindent(
                indent,
                ('class %(name)s_tag {};\n'+
                 'typedef ::xju::Int<%(name)s_tag,%(aliasOf)s> %(name)s;')%vars())
        elif aliasOf.kind() in basicFloatTypes:
            aliasOf=unqualifiedType(aliasOf)
            result=reindent(
                indent,
                ('class %(name)s_tag {};\n'+
                 'typedef ::xju::Float<%(aliasOf)s,%(name)s_tag> %(name)s;')%vars())
        elif aliasOf.kind() in basicStringTypes:
            result=reindent(
                indent,
                ('class %(name)s_tag {};\n'+
                 'typedef ::xju::Tagged<std::string,%(name)s_tag> %(name)s;')%vars())
        else:
            aliasOf=unqualifiedType(aliasOf)
            result=reindent(
                indent,
                ('typedef %(aliasOf)s %(name)s;')%vars())
            pass
    elif isinstance(decl, idlast.Struct):
        name=decl.identifier()
        memberTypesAndNames=[(unqualifiedType(_.memberType()),_.declarators()[0].identifier()) for _ in decl.members()];
        result=reindent(
            indent,
            gen_struct(name,memberTypesAndNames))
        pass
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
        result=result+sum([tincludes(p.paramType()) for p in decl.parameters()],[])
        result=result+tincludes(decl.returnType())
    elif isinstance(decl, idlast.Typedef):
        aliasOf=decl.aliasType()
        if aliasOf.kind() in basicIntTypes:
            result=result+['<xju/Int.hh>']+tincludes(aliasOf)
        elif aliasOf.kind() in basicFloatTypes:
            result=result+['<xju/Float.hh>']+tincludes(aliasOf)
        elif aliasOf.kind() in basicStringTypes:
            result=result+['<xju/Tagged.hh>','<string>']
            pass
        else:
            result=tincludes(aliasOf)
        pass
    elif isinstance(decl, idlast.Struct):
        for m in decl.members():
            if m.memberType().kind() in basicIntTypes:
                result=result+['<xju/Int.hh>']+tincludes(m.memberType())
            elif m.memberType().kind() in basicFloatTypes:
                result=result+['<xju/Float.hh>']+tincludes(m.memberType())
            elif m.memberType().kind() in basicStringTypes:
                result=result+['<xju/Tagged.hh>','<string>']
                pass
            pass
        pass
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
