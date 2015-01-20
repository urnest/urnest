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
virtual %(returnType)s %(name)s(%(params)s) throw(%(exceptions)s
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
    idltype.tk_octet:   TypeInfo('uint8_t',['<stdint.h>']),
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
    if p.direction()==0: #in
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
  friend bool operator<(
    %(name)s const& x, 
    %(name)s const& y) throw() {%(lessMembers)s
    return false;
  }
  friend bool operator>(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return (x>y)||(x==y);
  }
};
'''

pair_t='''\
struct %(name)s : public std::pair< %(t1)s, %(t2)s>
{
  %(name)s(%(t1)s const& p1, %(t2)s const& p2) throw():
     std::pair< %(t1)s, %(t2)s>(p1, p2) {
  }
  template<class T1, class T2>
  explicit %(name)s(std::pair<T1, T2> const& x) throw():
     std::pair< %(t1)s, %(t2)s>(x) {
  }
  friend bool operator<(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    if (x.first < y.first) return true;
    if (y.first < x.first) return false;
    return false;
  }
  friend bool operator>(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return (x>y)||(x==y);
  }
};
'''

def gen_pair(name, t1, t2):
    return pair_t%vars()

def gen_struct(name,memberTypesAndNames):
    assert len(memberTypesAndNames)>0, name
    memberNames=[_[1] for _ in memberTypesAndNames]
    memberTypes=[_[0] for _ in memberTypesAndNames]
    if name.endswith('Pair') and \
            tuple(memberNames)==tuple(['first','second']):
        return gen_pair(name, memberTypes[0], memberTypes[1])
    paramNames=['p%s'%i for i in range(1,len(memberTypesAndNames)+1)]
    
    members=''.join(['\n  %s %s;'%_ for _ in memberTypesAndNames])
    consparams=','.join(['\n    %s const& %s'%_ for _ in zip(memberTypes,paramNames)])
    consinitialisers=','.join(['\n      %s(%s)'%_ for _ in zip(memberNames,paramNames)])
    lessMembers=''.join([('\n    if (x.%(_)s<y.%(_)s) return true;'+
                          '\n    if (y.%(_)s<x.%(_)s) return false;')%vars()\
                             for _ in memberNames])
    return struct_t%vars()

#see also mapped_exception_t below
exception_t='''\
struct %(name)s : %(eclass)s
{
  %(name)s(%(consparams)s
    // %(eclass)s params
    std::string const& cause, 
    std::pair<std::string, unsigned int> const& fileAndLine) throw():
      %(eclass)s(cause, fileAndLine)%(consinitialisers)s {
  }
  %(members)s
  friend bool operator<(
    %(name)s const& x, 
    %(name)s const& y) throw() {%(lessMembers)s
    return (%(eclass)s const&)x < (%(eclass)s const&)y;
  }
  friend bool operator>(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return (x>y)||(x==y);
  }
};
'''


def gen_exception(name,memberTypesAndNames,eclass):
    memberNames=[_[1] for _ in memberTypesAndNames]
    memberTypes=[_[0] for _ in memberTypesAndNames]
    paramNames=['p%s'%i for i in range(1,len(memberTypesAndNames)+1)]
    
    members=''.join(['\n  %s %s;'%_ for _ in memberTypesAndNames])
    consparams=''.join(['\n    %s const& %s,'%_ for _ in zip(memberTypes,paramNames)])
    consinitialisers=''.join([',\n      %s(%s)'%_ for _ in zip(memberNames,paramNames)])
    lessMembers=''.join([('\n    if (x.%(_)s<y.%(_)s) return true;'+
                          '\n    if (y.%(_)s<x.%(_)s) return false;')%vars()\
                             for _ in memberNames])
    return exception_t%vars()

#see also exception_t above
mapped_exception_t='''\
struct %(name)s : %(eclass)s
{
  %(name)s(%(consparams)s) throw():
      %(eclass)s(
        %(causeParamName)s.first, 
        %(causeParamName)s.second)%(consinitialisers)s {
    cxy::copyContext(%(contextParamName)s.begin(), 
                     %(contextParamName)s.end(), 
                     *this);
  }

  // pre: %(eclass)s(T) constructs a %(eclass)s
  template<class T>
  explicit E1(
    T const& p) throw():
      %(eclass)s(p) {
  }
  
  // pre: %(eclass)s(T1,T2) constructs a %(eclass)s
  template<class T1, class T2>
  explicit E1(
    T1 const& p1,
    T2 const& p2) throw():
      %(eclass)s(p1,p2) {
  }
  %(members)s
  friend bool operator<(
    %(name)s const& x, 
    %(name)s const& y) throw() {%(lessMembers)s
    return (%(eclass)s const&)x < (%(eclass)s const&)y;
  }
  friend bool operator>(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    %(name)s const& x, 
    %(name)s const& y) throw() {
    return (x>y)||(x==y);
  }
};
'''

def gen_mapped_exception(name,
                         memberTypesAndNames,
                         eclass,
                         causeType,
                         contextType):
    memberNames=[_[1] for _ in memberTypesAndNames]
    memberTypes=[_[0] for _ in memberTypesAndNames]
    causeIndex=memberTypes.index(causeType)
    contextIndex=memberTypes.index(contextType)
    paramNames=['p%s'%i for i in range(1,len(memberTypesAndNames)+1)]
    causeParamName=paramNames[causeIndex]
    contextParamName=paramNames[contextIndex]
    members=['\n  %s %s;'%_ for _ in memberTypesAndNames]
    consparams=','.join(['\n    %s const& %s'%_ for _ in zip(memberTypes,paramNames)])
    consinitialisers=[',\n      %s(%s)'%_ \
                          for _ in zip(memberNames,paramNames)]
    i=[causeIndex,contextIndex]
    i.sort()
    del consinitialisers[i[1]]
    del consinitialisers[i[0]]
    consinitialisers=''.join(consinitialisers)
    del members[i[1]]
    del members[i[0]]
    members=''.join(members)
    del memberNames[i[1]]
    del memberNames[i[0]]
    lessMembers=''.join(\
        [('\n    if (x.%(_)s<y.%(_)s) return true;'+\
              '\n    if (y.%(_)s<x.%(_)s) return false;')%vars()\
             for _ in memberNames])

    return mapped_exception_t%vars()

def gen(decl,eclass,eheader,causeType,contextType,indent=''):
    result=''
    if isinstance(decl, idlast.Module):
        ns=decl.identifier()
        result='%(indent)snamespace %(ns)s\n%(indent)s{\n'%vars()
        result=result+''.join(
            [gen(_,eclass,eheader,causeType,contextType,indent)+'\n' \
                 for _ in decl.definitions()])
        result=result+'%(indent)s}'%vars()
    elif isinstance(decl, idlast.Interface):
        name=decl.identifier()
        content='\n'.join(
            [gen(_,eclass,eheader,causeType,contextType,indent+'  ') \
                 for _ in decl.contents()])
        result=reindent(indent,interface_t%vars())
    elif isinstance(decl, idlast.Operation):
        name=decl.identifier()
        params=','.join(['\n  %s& %s'%(ptype(p),p.identifier()) \
                             for p in decl.parameters()])
        assert not decl.oneway(), 'oneway not yet implemented'
        assert len(decl.contexts())==0, 'contexts not yet implemented'
        exceptionTypes=['::'.join(_.scopedName()) for _ in decl.raises()]
        exceptions=''.join(['\n  %(_)s,'%vars() for _ in exceptionTypes])
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
    elif isinstance(decl, idlast.Exception):
        name=decl.identifier()
        memberTypesAndNames=[\
            (unqualifiedType(_.memberType()),_.declarators()[0].identifier()) \
                for _ in decl.members()];
        memberTypes=[_[0] for _ in memberTypesAndNames]
        if causeType in memberTypes and contextType in memberTypes:
            result=reindent(
                indent,
                gen_mapped_exception(name,memberTypesAndNames,
                                     eclass,causeType,contextType))
        else:
            result=reindent(
                indent,
                gen_exception(name,memberTypesAndNames,eclass))
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
    elif isinstance(decl, idlast.Exception):
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

#include <cxy/copyContext.hh> //impl
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
    eclass,eheader=([_.split('-e',1)[1].split('=',1) for _ in args \
                         if _.startswith('-e')]+\
                        [('cxy::Exception','cxy/Exception.hh')])[0]
    causeType=([_.split('-causeType=',1)[1] for _ in args \
                                if _.startswith('-causeType')]+\
                               [None])[0]
    contextType=([_.split('-contextType=',1)[1] for _ in args \
                                if _.startswith('-contextType')]+\
                               [None])[0]
    if eheader.startswith('./') or os.path.dirname(eheader)=='':
        eheader='"%s"'%eheader[2:]
    else:
        eheader='<%s>'%eheader
    fileName=os.path.basename(tree.file())
    tincludes='\n'.join(['#include '+_ for _ in set(sum([gen_tincludes(_) for _ in tree.declarations() if _.mainFile()],[]))])
    idlincludes=gen_idlincludes(set([_.file() for _ in tree.declarations() if not _.mainFile()]))
    
    print head % vars()
    print '\n'.join([gen(_,eclass,eheader,causeType,contextType) \
                         for _ in tree.declarations() if _.mainFile()])
    pass
