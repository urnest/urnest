from omniidl import idlast
from omniidl import idltype

import sys
import os.path

from cxy import unqualifiedType,GenerateFailed,basicIntTypes
from cxy import get_union_cases

interface_t='''\
'''

struct_t='''\
template<>
struct TypeCodeOf< ::%(name)s >
{
  static std::shared_ptr<cxy::TypeCode> create() throw(std::bad_alloc)
  {
    return std::shared_ptr<cxy::TypeCode>(
      new cxy::StructTypeCode(
        cxy::cdr< ::%(name)s >::repoId,
        "%(name)s",
        {%(struct_members)s
        }));
  }
};
'''
struct_member_t='''\n          { "%(member_name)s", cxy::TypeCodeOf< %(member_type)s >::create() }'''
def gen_struct(name,memberTypesAndNames,repoId):
    assert len(memberTypesAndNames)>0, name
    struct_members=''.join(
        [struct_member_t%vars() 
         for member_type,member_name in memberTypesAndNames])
    return struct_t%vars()

union_t='''\
template<>
struct TypeCodeOf< ::%(name)s >
{
  static std::shared_ptr<cxy::TypeCode> create() throw(std::bad_alloc)
  {
    auto marshalCaseValue( [](%(descriminateType)s const& x ) {
      xju::MemCdrStream s;
      cxy::cdr< %(descriminateType)s >::marshal(x,s);
      return s;
    });
    return std::shared_ptr<cxy::TypeCode>(
      new cxy::UnionTypeCode(
        cxy::cdr< ::%(name)s >::repoId,
        "%(name)s",
        {
          %(union_cases)s
        },
        %(defaultCase)s));
  }
};
'''
union_case_t='''\n          { marshalCaseValue(%(case_value)s),"%(case_name)s", cxy::TypeCodeOf< %(case_type)s >::create() }'''
def gen_union(name,discriminateType,cases,repoId):
    assert len(cases)>0, name
    defaultCase='xju::Optional<uint32_t>()'
    if cases[-1][0] is None:
        assert len(cases)>1, name
        cases[-1]=(cases[0][0],cases[-1][1])
        l=len(cases)
        defaultCase='xju::Optional<uint32_t>(%(l)s)'%vars()
        pass
    union_cases=',\n          '.join(
        [union_case_t%vars() 
         for case_value,case_type,case_name in cases])
    return union_t%vars()

#see also mapped_exception_t below
exception_t='''\
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

enum_t='''\
  REVISIT
'''

def gen(decl,eclass,eheader,causeType,contextType,
        causeMemberExpression,contextMemberExpression,indent=''):
    try:
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
            repoId=decl.repoId()
            name='::'.join(decl.scopedName())
            memberTypesAndNames=[(unqualifiedType(_.memberType(),eclass),_.declarators()[0].identifier()) for _ in decl.members()];
            result=gen_struct(name,memberTypesAndNames,repoId)
            pass
        elif isinstance(decl, idlast.Union):
            repoId=decl.repoId()
            name='::'.join(decl.scopedName())
            switchTypeName=unqualifiedType(decl.switchType(),eclass)
            cases=get_union_cases(decl,eclass)
            result=gen_union(name,descriminateType,cases,repoId)
            pass
        elif isinstance(decl, idlast.Exception):
            name='::'.join(decl.scopedName())
            repoId=decl.repoId()
            memberTypesAndNames=[
                (unqualifiedType(_.memberType(),eclass),_.declarators()[0].identifier()) \
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
        elif isinstance(decl, idlast.Enum):
            name='::'.join(decl.scopedName())
            repoId=decl.repoId()
            result=enum_t%vars()
        elif isinstance(decl, idlast.Const):
            pass
        else:
            assert False, repr(decl)
            pass
        return result
    except:
        raise GenerateFailed(decl,sys.exc_info())
    pass

template='''\
// generated from %(fileName)s by omnicxy cxyany.py idl backend

#include <cxy/TypeTag.hh>
#include <cxy/TypeCode.hh>

#include <cxy/any_.hh>

#include %(hhinc)s
#include %(hhcdrinc)s

namespace cxy
{
%(items)s
}
'''

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
    hhext=([_.split('-hhext=',1)[1] for _ in args \
            if _.startswith('-hhext')]+\
           ['hh'])[0]
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
    hpath=([_.split('-hpath=',1)[1] for _ in args \
                if _.startswith('-hpath')]+\
               [''])[0]
    if len(hpath)>0 and not hpath.endswith('/'):
        hpath=hpath+'/'
    if len(hpath):
        hhinc='<%(hpath)s%(baseName)s.%(hhext)s>'%vars()
        hhcdrinc='<%(hpath)s%(baseName)s.cdr.%(hhext)s>'%vars()
    else:
        hhinc='"%(baseName)s.%(hhext)s"'%vars()
        hhcdrinc='"%(baseName)s.cdr.%(hhext)s"'%vars()
        pass
    print template % vars()
    pass
