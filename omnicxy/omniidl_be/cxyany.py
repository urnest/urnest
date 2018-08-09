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
  static ::cxy::TypeCode create(TypeCodeRefIndex& index) throw(std::bad_alloc)
  {
    return ::cxy::TypeCode(
      std::shared_ptr<cxy::TypeCode_>(
        new cxy::StructTypeCode(
          cxy::cdr< ::%(name)s >::repoId,
          "%(name)s",
          {%(struct_members)s
          })));
  }
};
'''
struct_member_t='''\n          { "%(member_name)s", ::cxy::createTypeCodeOf< %(member_type)s >(index) }'''
def gen_struct(name,memberTypesAndNames,repoId):
    assert len(memberTypesAndNames)>0, name
    struct_members=','.join(
        [struct_member_t%vars() 
         for member_type,member_name in memberTypesAndNames])
    return struct_t%vars()

union_t='''\
template<>
struct TypeCodeOf< ::std::shared_ptr< ::%(name)s const > >
{
  static ::cxy::TypeCode create(TypeCodeRefIndex& index) throw(std::bad_alloc)
  {
    auto marshalCaseValue( [](%(discriminantType)s const& x ) {
      cxy::MemCdrStream s;
      cxy::cdr< %(discriminantType)s >::marshal(x,*s);
      return s;
    });
    return ::cxy::TypeCode(
      std::shared_ptr<cxy::TypeCode_>(
        new cxy::UnionTypeCode(
          cxy::cdr< ::std::shared_ptr< ::%(name)s const > >::repoId,
          "%(name)s",
          cxy::createTypeCodeOf< %(discriminantType)s >(index),
          {
            %(union_cases)s
          },
          %(defaultCase)s)));
  }
};
'''
union_case_t='''\n          { cxy::UnionTypeCode::Case(marshalCaseValue(%(case_value)s),"%(case_name)s", cxy::createTypeCodeOf< %(case_type)s >(index)) }'''
def gen_union(name,discriminantType,cases,repoId):
    assert len(cases)>0, name
    defaultCase='xju::Optional<uint32_t>()'
    if cases[-1][0] is None:
        assert len(cases)>1, name
        cases[-1]=(cases[0][0],cases[-1][1])
        l=len(cases)-1
        defaultCase='xju::Optional<uint32_t>(%(l)s)'%vars()
        pass
    union_cases=','.join(
        [union_case_t%vars() 
         for case_value,case_type,case_name in [(_[0],_[1][0][0],_[1][0][1]) for _ in cases]])
    return union_t%vars()

exception_t='''\
template<>
struct TypeCodeOf< ::%(name)s >
{
  static ::cxy::TypeCode create(TypeCodeRefIndex& index) throw(std::bad_alloc)
  {
    return ::cxy::TypeCode(
      std::shared_ptr<cxy::TypeCode_>(
        new cxy::ExceptionTypeCode(
          cxy::cdr< ::%(name)s >::repoId,
          "%(name)s",
          {%(exception_members)s
          })));
  }
};
'''
exception_member_t='''\n          { "%(member_name)s", ::cxy::createTypeCodeOf< %(member_type)s >(index) }'''
def gen_exception(name,memberTypesAndNames,repoId):
    #assert len(memberTypesAndNames)>0, name
    exception_members=','.join(
        [exception_member_t%vars() 
         for member_type,member_name in memberTypesAndNames])
    return exception_t%vars()


enum_t='''\
template<>
struct TypeCodeOf< ::%(name)s >
{
  static ::cxy::TypeCode create(TypeCodeRefIndex& index) throw(std::bad_alloc)
  {
    return ::cxy::TypeCode(
      std::shared_ptr<cxy::TypeCode_>(
        new cxy::EnumTypeCode(
          cxy::cdr< ::%(name)s >::repoId,
          "%(name)s",
          {%(enum_members)s
          })));
  }
};
'''
enum_member_t='''\n          { "%(value)s" }'''
def gen_enum(name,values,repoId):
    enum_members=','.join(
        [enum_member_t%vars() 
         for value in values])
    return enum_t%vars()

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
            if decl.switchType().kind()==idltype.tk_enum:
                #qualify the label (case) values
                q=decl.switchType().scopedName()
                cases=[('::'+'::'.join(q+[_[0]]) if not _[0] is None else None,
                        _[1])
                       for _ in cases]
                pass
            result=gen_union(name,switchTypeName,cases,repoId)
            pass
        elif isinstance(decl, idlast.Exception):
            name='::'.join(decl.scopedName())
            repoId=decl.repoId()
            memberTypesAndNames=[
                (unqualifiedType(_.memberType(),eclass),_.declarators()[0].identifier()) \
                    for _ in decl.members()];
            memberTypes=[_[0] for _ in memberTypesAndNames]
            result=gen_exception(name,memberTypesAndNames,repoId)
            pass
        elif isinstance(decl, idlast.Enum):
            name='::'.join(decl.scopedName())
            repoId=decl.repoId()
            values=[_.identifier() for _ in decl.enumerators()]
            result=gen_enum(name,values,repoId)
            pass
        elif isinstance(decl, idlast.Const):
            pass
        elif isinstance(decl, idlast.Forward):
            pass
        elif isinstance(decl, idlast.StructForward):
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
