from omniidl import idlast
from omniidl import idltype

import sys
import os.path

from cxy import unqualifiedType

interface_t='''\
template<>
class cdr< ::%(fqn)s >
{
public:
  static const char repoId[]="%(repoId)s";
};
'''

struct_t='''\
template<>
class cdr< ::%(name)s >
{
public:
  static ::%(name)s unmarshalFrom(cdrStream& s) 
  //to avoid needing CORBA.h in our .hh, excepiton specs are commented
  //throw(
  //  CORBA::SystemException
  //  )
  {%(memberUnmarshals)s
    return ::%(name)s(%(consparams)s);
  }  
  static void marshal(%(name)s const& x, cdrStream& s) throw()
  {%(memberMarshals)s
  }
};
'''
def gen_struct(name,memberTypesAndNames):
    assert len(memberTypesAndNames)>0, name
    memberNames=[_[1] for _ in memberTypesAndNames]
    memberTypes=[_[0] for _ in memberTypesAndNames]
    paramNames=['p%s'%i for i in range(1,len(memberTypesAndNames)+1)]
    memberUnmarshals=''.join(['\n    %(t)s const %(pn)s(cdr< %(t)s >::unmarshalFrom(s));'%vars() for t,pn in zip(memberTypes,paramNames)])
    consparams=', '.join([pn for pn in paramNames])
    memberMarshals=''.join(['\n    cdr< %(t)s >::marshal(x.%(n)s,s);'%vars() for t,n in zip(memberTypes,memberNames)])
    return struct_t%vars()

#see also mapped_exception_t below
exception_t='''\
template<>
class cdr< ::%(name)s >
{
public:
  static ::%(name)s unmarshalFrom(cdrStream& s) 
  //to avoid needing CORBA.h in our .hh, excepiton specs are commented
  //throw(
  //  CORBA::SystemException
  //  )
  {%(memberUnmarshals)s
    return ::%(name)s(%(consparams)s
             //%(eclass)s params
             std::string("%(repoId)s"), std::make_pair(__FILE__,__LINE__));
  }  
  static void marshal(%(name)s const& x, cdrStream& s) throw()
  {%(memberMarshals)s
  }
  static const char repoId[]="%(repoId)s";
};
'''
def gen_exception(name,repoId,memberTypesAndNames,eclass):
    memberNames=[_[1] for _ in memberTypesAndNames]
    memberTypes=[_[0] for _ in memberTypesAndNames]
    paramNames=['p%s'%i for i in range(1,len(memberTypesAndNames)+1)]
    memberUnmarshals=''.join(['\n    %(t)s const %(pn)s(cdr< %(t)s >::unmarshalFrom(s));'%vars() for t,pn in zip(memberTypes,paramNames)])
    consparams=''.join([pn+',' for pn in paramNames])
    memberMarshals=''.join(['\n    cdr< %(t)s >::marshal(x.%(n)s,s);'%vars() for t,n in zip(memberTypes,memberNames)])
    return exception_t%vars()

#see also exception_t above
mapped_exception_t='''\
template<>
class cdr< ::%(name)s >
{
public:
  static ::%(name)s unmarshalFrom(cdrStream& s) 
  //to avoid needing CORBA.h in our .hh, excepiton specs are commented
  //throw(
  //  CORBA::SystemException
  //  )
  {%(memberUnmarshals)s
    return ::%(name)s(%(consparams)s);
  }  
  static void marshal(%(name)s const& x, cdrStream& s) throw()
  {%(memberMarshals)s
  }
  static const char repoId[]="%(repoId)s";
};
'''
def mapped_marshal(t,n,
                   causeType,
                   contextType,
                   causeMemberExpression,
                   contextMemberExpression):
    if t==causeType:
        causeMemberExpression=causeMemberExpression or n
        return '\n    cdr< %(t)s >::marshal(%(t)s(x.%(causeMemberExpression)s),s);'%vars()
    elif t==contextType:
        contextMemberExpression=contextMemberExpression or n
        return '\n    cdr< %(t)s >::marshal(%(t)s(x.%(contextMemberExpression)s.begin(),x.%(contextMemberExpression)s.end()),s);'%vars()
    return '\n    cdr< %(t)s >::marshal(x.%(n)s,s);'%vars() 
    
def gen_mapped_exception(name,repoId,memberTypesAndNames,
                         eclass,causeType,contextType,
                         causeMemberExpression,contextMemberExpression):
    memberNames=[_[1] for _ in memberTypesAndNames]
    memberTypes=[_[0] for _ in memberTypesAndNames]
    paramNames=['p%s'%i for i in range(1,len(memberTypesAndNames)+1)]
    memberUnmarshals=''.join(['\n    %(t)s const %(pn)s(cdr< %(t)s >::unmarshalFrom(s));'%vars() for t,pn in zip(memberTypes,paramNames)])
    consparams=','.join(paramNames)
    memberMarshals=''.join([mapped_marshal(t,n,
                                           causeType,contextType,
                                           causeMemberExpression,
                                           contextMemberExpression) \
                                for t,n in zip(memberTypes,memberNames)])
    return mapped_exception_t%vars()

def gen(decl,eclass,eheader,causeType,contextType,
        causeMemberExpression,contextMemberExpression,indent=''):
    result=''
    if isinstance(decl, idlast.Module):
        result=''.join(gen(_,eclass,eheader,causeType,contextType,
                           causeMemberExpression,contextMemberExpression) \
                           for _ in decl.definitions())
    elif isinstance(decl, idlast.Interface):
        fqn='::'.join(decl.scopedName())
        repoId=decl.repoId()
        result=interface_t%vars()+\
            ''.join(gen(_,eclass,eheader,causeType,contextType,
                        causeMemberExpression,contextMemberExpression) \
                        for _ in decl.contents())
    elif isinstance(decl, idlast.Operation):
        pass
    elif isinstance(decl, idlast.Typedef):
        pass
    elif isinstance(decl, idlast.Struct):
        name='::'.join(decl.scopedName())
        memberTypesAndNames=[(unqualifiedType(_.memberType()),_.declarators()[0].identifier()) for _ in decl.members()];
        result=gen_struct(name,memberTypesAndNames)
        pass
    elif isinstance(decl, idlast.Exception):
        name='::'.join(decl.scopedName())
        repoId=decl.repoId()
        memberTypesAndNames=[
            (unqualifiedType(_.memberType()),_.declarators()[0].identifier()) \
                for _ in decl.members()];
        memberTypes=[_[0] for _ in memberTypesAndNames]
        if causeType in memberTypes and contextType in memberTypes:
            result=gen_mapped_exception(
                name,repoId,memberTypesAndNames,
                eclass,causeType,contextType,
                causeMemberExpression,contextMemberExpression)
        else:
            result=gen_exception(name,repoId,memberTypesAndNames,eclass)
            pass
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
    causeType=([_.split('-causeType=',1)[1] for _ in args \
                                if _.startswith('-causeType')]+\
                               [None])[0]
    contextType=([_.split('-contextType=',1)[1] for _ in args \
                                if _.startswith('-contextType')]+\
                               [None])[0]
    causeMemberExpression=(
        [_.split('-causeMemberExpression=',1)[1] for _ in args \
             if _.startswith('-causeMemberExpression')]+\
            [None])[0]
    contextMemberExpression=(\
        [_.split('-contextMemberExpression=',1)[1] for _ in args \
             if _.startswith('-contextMemberExpression')]+\
            [None])[0]
    if eheader.startswith('./') or os.path.dirname(eheader)=='':
        eheader='"%s"'%eheader[2:]
    else:
        eheader='<%s>'%eheader
    assert tree.file().endswith('.idl'), tree.file()
    fileName=os.path.basename(tree.file())
    baseName=fileName[0:-4]
    items=''.join([gen(_,eclass,eheader,causeType,contextType,
                       causeMemberExpression,contextMemberExpression) \
                       for _ in tree.declarations() \
                       if _.mainFile()])
    idlincludes=gen_idlincludes(set([_.file() for _ in tree.declarations() if not _.mainFile()]))
    print template % vars()
    pass
