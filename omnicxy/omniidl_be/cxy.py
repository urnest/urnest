from omniidl import idlast
from omniidl import idltype

import sys
import os.path
import types
import traceback

class GenerateFailed(Exception):
    def __init__(self,decl,cause):
        self.decl=decl
        self.cause=cause #sys.exc_info()
        pass
    def __str__(self):
        decl=self.decl
        file=self.decl.file()
        line=self.decl.line()
        cause=''.join(traceback.format_exception(*self.cause))
        return 'failed to generate decl %(decl)r from file %(file)r line %(line)s because\n%(cause)s' %vars()
    pass

interface_t='''\
class %(name)s %(inherits)s
{
public:
  virtual ~%(name)s() throw()
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
    idltype.tk_char:   TypeInfo('char',[]),
    idltype.tk_octet:   TypeInfo('uint8_t',['<stdint.h>']),
    idltype.tk_short:   TypeInfo('int16_t',['<stdint.h>']),
    idltype.tk_long:    TypeInfo('int32_t',['<stdint.h>']),
    idltype.tk_longlong:    TypeInfo('int64_t',['<stdint.h>']),
    idltype.tk_ushort:  TypeInfo('uint16_t',['<stdint.h>']),
    idltype.tk_ulong:   TypeInfo('uint32_t',['<stdint.h>']),
    idltype.tk_ulonglong:   TypeInfo('uint64_t',['<stdint.h>']),
    idltype.tk_boolean:   TypeInfo('bool',[]),
}
basicFloatTypes={
    idltype.tk_float: TypeInfo('float',[]),
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

def sequenceUnqualifiedType(t,eclass):
    bound=t.bound()
    if bound==0:
        itemType=unqualifiedType(t.seqType(),eclass)
        return 'std::vector< %(itemType)s >'%vars()
    elif bound==1:
        itemType=unqualifiedType(t.seqType(),eclass)
        return 'cxy::optional< %(itemType)s >'%vars()
    else:
        itemType=unqualifiedType(t.seqType(),eclass)
        return 'cxy::BoundedSequence<%(itemType)s,%(bound)s>'%vars()
    pass

def unionUnqualifiedType(t,eclass):
    t=''.join(['::'+_ for _ in t.scopedName()])
    return '::std::shared_ptr< %(t)s const>'%vars()

def objrefUnqualifiedType(t,eclass):
    t=''.join(['::'+_ for _ in t.scopedName()])
    if t=='::CORBA::Object': t='void'
    return '::cxy::IOR< %(t)s >'%vars()

unqualifiedType_=dict(
    [(idltype.tk_any,lambda t,eclass:'::cxy::Any< {eclass} >'.format(**vars()))]+
    [(idltype.tk_TypeCode,lambda t,eclass:'::cxy::TypeCode')]+
    [(kind,lambda t,eclass:basicParamTypes.get(t.kind()).typename)
     for kind in basicParamTypes]+
    [(kind,lambda t,eclass:''.join(['::'+_ for _ in t.scopedName()]))
     for kind in [idltype.tk_alias,
                  idltype.tk_struct,
                  idltype.tk_enum,
                  idltype.ot_structforward]]+
    [(idltype.tk_sequence,sequenceUnqualifiedType)]+
    [(idltype.tk_union,unionUnqualifiedType)]+
    [(idltype.tk_objref,objrefUnqualifiedType)])
               
def unqualifiedType(t,eclass):
    assert isinstance(t,idltype.Type),t
    if t.kind() in unqualifiedType_:
        return unqualifiedType_[t.kind()](t,eclass)
    assert False, '%s not implemented, only types %s implemented' % (t.kind(),unqualifiedType_.keys())
    pass

def unionPtype(p,eclass):
    assert p.paramType().kind()==idltype.tk_union, p.paramType().kind()
    unionTypeName=''.join(['::'+_ for _ in p.paramType().scopedName()])
    return '::std::shared_ptr< %(unionTypeName)s const>'%vars()
def objrefPtype(p,eclass):
    assert p.paramType().kind()==idltype.tk_objref, p.paramType().kind()
    t=''.join(['::'+_ for _ in p.paramType().scopedName()])
    if t=='::CORBA::Object': t='void'
    return '::cxy::IOR< %(t)s >'%vars()

ptype_=dict(
    [(idltype.tk_any, lambda p,eclass:'::cxy::Any< {eclass} >'.format(**vars()))]+
    [(idltype.tk_TypeCode,lambda p,eclass: '::cxy::TypeCode')]+
    [(idltype.tk_union,unionPtype)]+
    [(idltype.tk_objref,objrefPtype)]+
    [(kind,lambda p,eclass:''.join(['::'+_ for _ in p.paramType().scopedName()]))
     for kind in [idltype.tk_alias,
                  idltype.tk_struct,
                  idltype.tk_enum]]+
    [(kind, lambda p,eclass: basicParamTypes.get(p.paramType().kind()).typename)
     for kind in basicParamTypes])
    
def ptype(p,eclass):
    assert isinstance(p,idlast.Parameter),p
    if p.direction()==0: #in
        if p.paramType().kind() in ptype_:
            return ptype_[p.paramType().kind()](p,eclass)+' const&'
        assert False, '%s not implemented, only types %s implemented' % (p.paramType().kind(),ptype_.keys())
    assert False,p.dirtext()+'('+str(p.direction())+') params not yet implemented'

def tn(t):
    if type(t) is types.InstanceType:
        return t.__class__
    return type(t)

tincludes_=dict(
    [(idltype.tk_objref, lambda t: ['<cxy/IOR.hh>'])]+
    [(idltype.tk_any, lambda t: ['<cxy/Any.hh>'])]+
    [(idltype.tk_TypeCode, lambda t: ['<cxy/TypeCode.hh>'])]+
    [(kind, lambda t:basicParamTypes.get(t.kind()).includeFiles)
     for kind in basicParamTypes]+
    [(kind, lambda t: []) 
     for kind in [idltype.tk_alias,
                  idltype.tk_struct,
                  idltype.tk_union,
                  idltype.tk_enum,
                  idltype.ot_structforward]]+
    [(idltype.tk_sequence, lambda t: tincludes(t.seqType()))])

def tincludes(t):
    if t.kind() in tincludes_:
        return tincludes_[t.kind()](t)
    assert False, '%s (kind %s) not implemented, only types %s implemented' % (t,t.kind(),tincludes_.keys())

def pincludes(t):
    if t.kind() in [idltype.tk_union]:
        result=['<memory>']
        return result
    return tincludes(t)

def reindent(indent, s):
    '''prepend %(indent)r to each line of %(s)r'''
    return '\n'.join([indent+_ for _ in s.split('\n')])

struct_t='''\
struct %(name)s
{
  %(members)s

  %(name)s(%(consparams)s) throw():%(consinitialisers)s {
  }
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
struct %(name)s : public std::pair< %(t1)s, %(t2)s >
{
  %(name)s(%(t1)s const& p1, %(t2)s const& p2) throw():
     std::pair< %(t1)s, %(t2)s >(p1, p2) {
  }
  template<class T1, class T2>
  explicit %(name)s(std::pair<T1, T2> const& x) throw():
     std::pair< %(t1)s, %(t2)s >(x.first,%(t2)s(x.second)) {
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
  %(members)s

  ~%(name)s() throw(){}

  // pre: %(eclass)s(T) constructs a %(eclass)s
  template<class T>
  explicit %(name)s(%(consparams)s
    T const& p) throw():
      %(eclass)s(p)%(consinitialisers)s {
  }
  
  %(name)s(%(consparams)s
    // %(eclass)s params
    std::string const& cause, 
    std::pair<std::string, unsigned int> const& fileAndLine) throw():
      %(eclass)s(cause, fileAndLine)%(consinitialisers)s {
  }
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
  %(members)s

  ~%(name)s() throw(){}

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
  explicit %(name)s(
    T const& p) throw():
      %(eclass)s(p) {
  }
  
  // pre: %(eclass)s(T1,T2) constructs a %(eclass)s
  template<class T1, class T2>
  explicit %(name)s(
    T1 const& p1,
    T2 const& p2) throw():
      %(eclass)s(p1,p2) {
  }
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

enum_t='''\
class %(name)s
{
public:
  enum Value {%(valdecls)s
  };
  %(name)s(Value v) throw(%(eclass)s):
    v_(v) {
    if ((v < %(minValue)s)||(v > %(maxValue)s)){
      std::ostringstream s;
      s << v << " is outside allowed range %(minValue)s..%(maxValue)s";
      throw %(eclass)s(s.str(), std::make_pair(__FILE__,__LINE__));
    }
  }
  friend Value valueOf(%(name)s const& x) throw(){ return x.v_; }
private:
  Value v_;
  friend bool operator<(%(name)s const& x, %(name)s const& y) throw(){
    return x.v_ < y.v_;
  }
  friend bool operator>(%(name)s const& x, %(name)s const& y) throw(){
    return x.v_ > y.v_;
  }
  friend bool operator==(%(name)s const& x, %(name)s const& y) throw(){
    return x.v_ == y.v_;
  }
  friend bool operator!=(%(name)s const& x, %(name)s const& y) throw(){
    return x.v_ != y.v_;
  }
  friend bool operator<=(%(name)s const& x, %(name)s const& y) throw(){
    return x.v_ <= y.v_;
  }
  friend bool operator>=(%(name)s const& x, %(name)s const& y) throw(){
    return x.v_ >= y.v_;
  }
  friend std::ostream& operator<<(std::ostream& s, %(name)s const& x) throw(){
    switch(x.v_){%(oscases)s
    }
    return s << (x.v_);
  }
};
'''
def gen_enum(name,values,eclass):
    minValue=0
    maxValue=len(values)
    valdecls=','.join(['\n    %s'%_ for _ in values])
    oscases=''.join(['\n    case %(_)s: return s << "%(_)s";'%vars() \
                         for _ in values])
    return enum_t%vars()

union_case_def_t='''
%(templateIntro)sclass %(extraScope)s%(typeName)s::%(caseName)s : 
  public %(typeName)s
{
public:%(members)s

  virtual ~%(caseTypeNameInDecl)s() throw() {
  }
  explicit %(caseTypeNameInDecl)s(%(consparams)s) throw():
      %(typeName)s(%(descriminator)s)%(consinitialisers)s {
  }
  std::unique_ptr< %(extraScope)s%(typeName)s > clone() const // override 
  {
    return std::unique_ptr< %(typeName)s >(new %(caseName)s(*this));
  }
  friend bool operator<(
    %(caseName)s const& x, 
    %(caseName)s const& y) throw() {%(lessMembers)s
    return false;
  }
  friend bool operator>(
    %(caseName)s const& x, 
    %(caseName)s const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    %(caseName)s const& x, 
    %(caseName)s const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    %(caseName)s const& x, 
    %(caseName)s const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    %(caseName)s const& x, 
    %(caseName)s const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    %(caseName)s const& x, 
    %(caseName)s const& y) throw() {
    return (x>y)||(x==y);
  }
  bool lessThan(%(typeName)s const& b) const throw()
  {
    if (dynamic_cast<%(typeName)s::%(caseName)s const* >(&b)) {
      return (*this) < dynamic_cast< %(typeName)s::%(caseName)s const& >(b);
    }%(latter_cases)s
    return false;
  }
};
'''
def gen_union_case_def(typeName,
                       caseName,
                       latterCaseNames,
                       memberTypesAndNames,
                       switchTypeName,
                       descriminator,
                       templateIntro='',
                       caseTypeNameInDecl=None,
                       extraScopeNames=[]):
    memberNames=[_[1] for _ in memberTypesAndNames]
    memberTypes=[_[0] for _ in memberTypesAndNames]
    paramNames=['p%s'%i for i in range(1,len(memberTypesAndNames)+1)]
    
    members=''.join(['\n  %s %s;'%_ for _ in memberTypesAndNames])
    consparams=','.join(['\n    %s const& %s'%_ for _ in zip(memberTypes,paramNames)])
    consinitialisers=','.join(['\n      %s(%s)'%_ for _ in zip(memberNames,paramNames)])
    if len(consinitialisers):
        consinitialisers=','+consinitialisers
        pass
    lessMembers=''.join([('\n    if (x.%(_)s<y.%(_)s) return true;'+
                          '\n    if (y.%(_)s<x.%(_)s) return false;')%vars()
                         for _ in memberNames])
    caseTypeNameInDecl=caseTypeNameInDecl or caseName
    latter_cases=''.join([gen_union_latter_case(typeName,_) \
                              for _ in latterCaseNames])
    extraScope=''.join([_+'::' for _ in extraScopeNames])
    return union_case_def_t%vars()

union_default_case_def_t='''
class %(extraScope)s%(typeName)s::Default : 
  public %(typeName)s
{
public:%(members)s

  virtual ~Default() throw() {
  }
  explicit Default(%(consparams)s) throw():
      %(typeName)s(d)%(consinitialisers)s {%(validateDiscriminator)s
  }
  std::unique_ptr< %(extraScope)s%(typeName)s > clone() const //override 
  {
    return std::unique_ptr< %(typeName)s >(new Default(*this));
  }
  bool lessThan(%(typeName)s const& b) const throw() //override
  {
    if (dynamic_cast<%(typeName)s::Default const* >(&b)) {
      return (*this) < dynamic_cast< %(typeName)s::Default const& >(b);
    }
    return false;
  }
  friend bool operator<(
    Default const& x, 
    Default const& y) throw() {
      if (discriminator(x) < discriminator(y)) return true;%(lessMembers)s
      return false;
  }
  friend bool operator>(
    Default const& x, 
    Default const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    Default const& x, 
    Default const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    Default const& x, 
    Default const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    Default const& x, 
    Default const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    Default const& x, 
    Default const& y) throw() {
    return (x>y)||(x==y);
  }
};
'''
def gen_union_default_case_def(typeName,
                               memberTypesAndNames,
                               switchTypeName,
                               nonDefaultDiscriminators,
                               extraScopeNames):
    memberNames=[_[1] for _ in memberTypesAndNames]
    memberTypes=[_[0] for _ in memberTypesAndNames]
    paramNames=['p%s'%i for i in range(1,len(memberTypesAndNames)+1)]
    
    members=''.join(['\n  %s %s;'%_ for _ in memberTypesAndNames])
    consparams=','.join(
        ['%(switchTypeName)s d'%vars()]+
        ['\n    %s const& %s'%_ for _ in zip(memberTypes,paramNames)])
    consinitialisers=','.join(['\n      %s(%s)'%_ for _ in zip(memberNames,paramNames)])
    if len(consinitialisers):
        consinitialisers=','+consinitialisers
        pass
    validateDiscriminator=''.join(
        ['\n    xju::assert_not_equal(d,%(_)s);'%vars()
         for _ in nonDefaultDiscriminators])
    lessMembers=''.join([('\n    if (x.%(_)s<y.%(_)s) return true;'+
                          '\n    if (y.%(_)s<x.%(_)s) return false;')%vars()
                         for _ in memberNames])
    extraScope=''.join([_+'::' for _ in extraScopeNames])
    return union_default_case_def_t%vars()

union_latter_case_t='''
    if (dynamic_cast<%(typeName)s::%(latterCaseName)s const* >(&b)) {
      return true;
    }'''
def gen_union_latter_case(typeName,latterCaseName):
    return union_latter_case_t%vars()

enum_union_t='''\
 // IDL Union %(typeName)s
class %(typeName)s
{
public:
  virtual ~%(typeName)s() throw() {
  }
  %(union_case_fwds)s
  // throws std::bad_alloc and exceptions of case-type copy constructor
  virtual std::unique_ptr< %(typeName)s > clone() const=0;
protected:
  explicit %(typeName)s(::%(switchTypeName)s d) throw():
    d_(d){
  }
private:
  ::%(switchTypeName)s d_;
  friend ::%(switchTypeName)s discriminator(%(typeName)s const& x) throw() {
    return x.d_;
  }
  virtual bool lessThan(%(typeName)s const& b) const throw()=0;

  friend bool operator<(%(typeName)s const& a, %(typeName)s const& b) throw()
  {
    return a.lessThan(b);
  }
  friend bool operator>(%(typeName)s const& a, %(typeName)s const& b) throw()
  {
    return b < a;
  }
  friend bool operator!=(%(typeName)s const& a, %(typeName)s const& b) throw()
  {
    return (a<b)||(b<a);
  }
  friend bool operator==(%(typeName)s const& a, %(typeName)s const& b) throw()
  {
    return !(a<b)&&!(b<a);
  }
  friend bool operator<=(%(typeName)s const& a, %(typeName)s const& b) throw()
  {
    return (a<b)||(a==b);
  }
  friend bool operator>=(%(typeName)s const& a, %(typeName)s const& b) throw()
  {
    return (a>b)||(a==b);
  }
};
'''
def get_union_cases(decl,eclass):
    '''get union cases as [(label,[(memberType,memberName)]'''
    '''order is declaration order, except that default, if '''
    '''any, is always last, with label None'''
    '''eg union(long) { 1: string x; default: short y; } ->'''
    '''   [(1,[('std::string', 'x')]), (None,[('uint16_t',x)])]'''
    '''eg union(En) { A: string x; default: short y; } ->'''
    '''   [('A',[('std::string', 'x')]), (None,[('uint16_t',x)])]'''
    assert isinstance(decl,idlast.Union),decl
    labels=[]
    cases={}
    for c in decl.cases():
        assert c.constrType()==False,c
        for l in c.labels():
            if l.default():
                label=None
            else:
                label=l.value()
                if isinstance(label,idlast.Enumerator):
                    label=label.identifier()
                    pass
                labels.append(label)
                pass
            cases.setdefault(label,[]).append(
                (unqualifiedType(c.caseType(),eclass),#type
                 c.declarator().identifier())) #name
            pass
        pass
    result=[(_,cases[_]) for _ in labels]
    if None in cases:
        result.append((None, cases[None]))
        pass
    return result

class ForNamespaceScope:
    def __init__(self,f):
        assert callable(f),repr(f)
        self.f=f
        self.extraScope=[]
        pass
    def __str__(self):
        extraScope=self.extraScope
        extraScope.reverse()
        return self.f(extraScope)
    pass

class GenResult:
    def __init__(self,code,forNamespaceScope):
        assert isinstance(code,str),code.__class__
        assert isinstance(forNamespaceScope,list),forNamespaceScope.__class__
        assert len([_ for _ in forNamespaceScope if not isinstance(_,ForNamespaceScope)])==0,repr(forNamespaceScope)
        self.code=code
        self.forNamespaceScope=forNamespaceScope
        pass
    def addScope(self,scope):
        '''add scope like "na"'''
        for x in self.forNamespaceScope:
            x.extraScope.append(scope)
            pass
        pass
    def genNamespaceScopeCode(self,indent):
        return reindent(indent,
                        ''.join([str(_) for _ in self.forNamespaceScope]))
    pass

def gen_enum_union(decl,eclass):
    typeName=decl.identifier()
    assert decl.switchType().kind()==idltype.tk_enum, decl.switchType()
    switchTypeName='::'.join(decl.switchType().scopedName())
    labels=[_.identifier() for _ in \
                decl.switchType().decl().enumerators()]
    cases=dict([(label,[]) for label in labels])
    for c in decl.cases():
        assert c.constrType()==False,c
        for l in c.labels():
            assert isinstance(l.value(),idlast.Enumerator),l.value()
            cases[l.value().identifier()].append(
                (unqualifiedType(c.caseType(),eclass),#type
                c.declarator().identifier()))    #name
        pass
    #cases is like [('A', [('int32_t','a_')]), ('B', [])]
    ds=dict([(_.identifier(),'::%(switchTypeName)s::'%vars()+_.identifier())\
                 for _ in decl.switchType().decl().enumerators()])
    union_case_fwds='\n  '.join(['class %(_)s;'%vars() for _ in labels])
    union_case_defs=lambda scopeNames:\
                     ''.join(
                         [gen_union_case_def(
                             typeName,
                             caseName,
                             labels[labels.index(caseName)+1:],
                             cases[caseName],
                             switchTypeName,
                             ds[caseName],
                             '',
                             None,
                             scopeNames)\
                          for caseName in labels])
    return GenResult(enum_union_t%locals(),[
        ForNamespaceScope(union_case_defs)])

non_enum_union_t='''\
 // IDL Union %(typeName)s
class %(typeName)s
{
public:
  virtual ~%(typeName)s() throw() {
  }
  template< %(switchTypeName)s >
  class V;
/*
  %(union_case_fwds)s
*/
  class Default;
protected:
  explicit %(typeName)s(::%(switchTypeName)s d) throw():
    d_(d){
  }
private:
  ::%(switchTypeName)s d_;
  friend ::%(switchTypeName)s discriminator(%(typeName)s const& x) throw() {
    return x.d_;
  }
  virtual bool lessThan(%(typeName)s const& b) const throw()=0;

  friend bool operator<(%(typeName)s const& a, %(typeName)s const& b) throw()
  {
    return a.lessThan(b);
  }
  friend bool operator>(%(typeName)s const& a, %(typeName)s const& b) throw()
  {
    return b < a;
  }
  friend bool operator!=(%(typeName)s const& a, %(typeName)s const& b) throw()
  {
    return (a<b)||(b<a);
  }
  friend bool operator==(%(typeName)s const& a, %(typeName)s const& b) throw()
  {
    return !(a<b)&&!(b<a);
  }
  friend bool operator<=(%(typeName)s const& a, %(typeName)s const& b) throw()
  {
    return (a<b)||(a==b);
  }
  friend bool operator>=(%(typeName)s const& a, %(typeName)s const& b) throw()
  {
    return (a>b)||(a==b);
  }
};
template< %(switchTypeName)s >
class %(typeName)s::V
{
private:
  V() throw(); // must specialise class
};
'''
def gen_non_enum_union(decl,eclass):
    '''returns a GenResult'''
    assert decl.switchType().kind() in basicIntTypes,decl
    typeName=decl.identifier()
    switchTypeName=unqualifiedType(decl.switchType(),eclass)
    cases=get_union_cases(decl,eclass)
    labels=[_[0] for _ in cases if not _[0] is None]
    caseClasses=['V< %(_)s >'%vars() for _ in labels]
    cases=dict(cases)
    cases.setdefault(None,[]) #ensure a default case
    #cases is like {None: [('std::string', 'x_')], 2: [('float', 'c_')], 1: [('int32_t', 'a_')]}
    union_case_fwds='\n  '.join(['template<> class V< %(_)s >;'%vars()
                                 for _ in labels]+
                                ['class Default;'])
    #defer generation of the template specialisations to nearest
    #namespace scope, scopeNames gives us the scope "delta" as a list
    #from outermost to innermost
    union_case_defs=lambda scopeNames:\
                     ''.join([
                         gen_union_case_def(
                             typeName,
                             caseClass,
                             caseClasses[caseClasses.index(caseClass)+1:]+
                             ['Default'],
                             cases[label],
                             switchTypeName,
                             label,
                             'template<>\n',
                             'V',
                             scopeNames)
                         for label,caseClass in zip(labels,caseClasses)]+
                             [gen_union_default_case_def(typeName,
                                                         cases[None],
                                                         switchTypeName,
                                                         labels,
                                                         scopeNames)])
    return GenResult(non_enum_union_t%locals(),[
        ForNamespaceScope(union_case_defs)])

def narry(baseType,sizes,eclass):
    '''generate n-dimensional array of baseType'''
    if not len(sizes): return baseType
    n=sizes[0]
    inner=narry(baseType,sizes[1:],eclass)
    return 'xju::Array< {inner},{n},{eclass} >'.format(**vars())

def gen(decl,eclass,eheader,causeType,contextType,indent=''):
    '''- returns a GenResult'''
    try:
        result=GenResult('',[])
        if isinstance(decl, idlast.Module):
            ns=decl.identifier()
            result.code='%(indent)snamespace %(ns)s\n%(indent)s{\n'%vars()
            genResults=[
                gen(_,eclass,eheader,causeType,contextType,indent) \
                for _ in decl.definitions()]
            result.code=result.code+''.join([_.code+'\n' for _ in genResults])
            result.code=result.code+''.join(
                [_.genNamespaceScopeCode(indent) for _ in genResults])
            result.code=result.code+'%(indent)s}'%vars()
        elif isinstance(decl, idlast.Interface):
            inherits=''
            if len(decl.inherits()):
                fqns=['::'.join(_.scopedName()) for _ in decl.inherits()]
                inherits=':'+','.join(
                    ['\n  public virtual %(fqn)s'%vars() for fqn in fqns])
                pass
            name=decl.identifier()
            genResults=[
                gen(_,eclass,eheader,causeType,contextType,indent+'  ')
                for _ in decl.contents()]
            content='\n'.join([_.code for _ in genResults])
            result.code=reindent(indent,interface_t%vars())
            result.forNamespaceScope=sum([_.forNamespaceScope for _ in genResults],[])
            result.addScope(decl.identifier())
        elif isinstance(decl, idlast.Operation):
            name=decl.identifier()
            params=','.join(['\n  %s %s'%(ptype(p,eclass),p.identifier()) \
                                 for p in decl.parameters()])
            assert len(decl.contexts())==0, 'contexts not yet implemented'
            exceptionTypes=['::'.join(_.scopedName()) for _ in decl.raises()]
            exceptions=''.join(['\n  %(_)s,'%vars() for _ in exceptionTypes])
            returnType=unqualifiedType(decl.returnType(),eclass)
            result.code=reindent(indent,operation_t%vars())
        elif isinstance(decl, idlast.Typedef):
            name=decl.declarators()[0].identifier()
            sizes=decl.declarators()[0].sizes()
            tagClass=''
            aliasOf=decl.aliasType()
            if aliasOf.kind() in basicIntTypes:
                tagClass='class %(name)s_tag {};\n'
                aliasOf=unqualifiedType(aliasOf,eclass)
                baseType='::xju::Int< %(name)s_tag,%(aliasOf)s,%(eclass)s > '
            elif aliasOf.kind() in basicFloatTypes:
                tagClass='class %(name)s_tag {};\n'
                aliasOf=unqualifiedType(aliasOf,eclass)
                baseType='::xju::Float< %(name)s_tag,%(aliasOf)s,%(eclass)s >'
            elif aliasOf.kind() in basicStringTypes:
                tagClass='class %(name)s_tag {};\n'
                baseType='::xju::Tagged<std::string,%(name)s_tag >'
            else:
                aliasOf=unqualifiedType(aliasOf,eclass)
                baseType='%(aliasOf)s'
                pass
            result.code=reindent(
                indent,
                (tagClass+'typedef '+narry(baseType,sizes,eclass)+' %(name)s;')%vars())
            pass
        elif isinstance(decl, idlast.Struct):
            name=decl.identifier()
            memberTypesAndNames=[(unqualifiedType(_.memberType(),eclass),_.declarators()[0].identifier()) for _ in decl.members()];
            result.code=reindent(
                indent,
                gen_struct(name,memberTypesAndNames))
            pass
        elif isinstance(decl, idlast.Exception):
            name=decl.identifier()
            memberTypesAndNames=[\
                (unqualifiedType(_.memberType(),eclass),_.declarators()[0].identifier()) \
                    for _ in decl.members()];
            memberTypes=[_[0] for _ in memberTypesAndNames]
            if causeType in memberTypes and contextType in memberTypes:
                result.code=reindent(
                    indent,
                    gen_mapped_exception(name,memberTypesAndNames,
                                         eclass,causeType,contextType))
            else:
                result.code=reindent(
                    indent,
                    gen_exception(name,memberTypesAndNames,eclass))
            pass
        elif isinstance(decl, idlast.Enum):
            name=decl.identifier()
            values=[_.identifier() for _ in decl.enumerators()]
            result.code=reindent(
                indent,
                gen_enum(name,values,eclass))
        elif isinstance(decl, idlast.Const):
            name=decl.identifier()
            type_=unqualifiedType(decl.constType(),eclass)
            if type_=='std::string':
                value='"%s"'%decl.value()
            else:
                value=repr(decl.value())
                pass
            result.code=reindent(
                indent,
                '%(type_)s const %(name)s = %(value)s;'%vars())
        elif isinstance(decl, idlast.Union):
            if decl.switchType().kind()==idltype.tk_enum:
                r=gen_enum_union(decl,eclass)
                result.code=reindent(indent, r.code)
                result.forNamespaceScope.extend(r.forNamespaceScope)
            else:
                r=gen_non_enum_union(decl,eclass)
                result.code=reindent(indent, r.code)
                result.forNamespaceScope.extend(r.forNamespaceScope)
        elif isinstance(decl, idlast.Forward):
            name=decl.identifier()
            result.code=reindent(
                indent,
                ('class %(name)s;')%vars())
            pass
        elif isinstance(decl, idlast.StructForward):
            name=decl.identifier()
            result.code=reindent(
                indent,
                ('class %(name)s;')%vars())
            pass
        else:
            assert False, repr(decl)
            pass
        return result
    except:
        raise GenerateFailed(decl,sys.exc_info())
    pass

def gen_tincludes(decl):
    result=[]
    if isinstance(decl, idlast.Module):
        result=result+sum([gen_tincludes(_) for _ in decl.definitions()],[])
    elif isinstance(decl, idlast.Interface):
        result=result+sum([gen_tincludes(_) for _ in decl.contents()],[])
    elif isinstance(decl, idlast.Operation):
        result=result+sum(
            [pincludes(p.paramType()) for p in decl.parameters()],[])
        result=result+pincludes(decl.returnType())
    elif isinstance(decl, idlast.Typedef):
        aliasOf=decl.aliasType()
        if aliasOf.kind() in basicIntTypes:
            result=result+['<xju/Int.hh>']+tincludes(aliasOf)
        elif aliasOf.kind() in basicFloatTypes:
            result=result+['<xju/Float.hh>']+tincludes(aliasOf)
        elif aliasOf.kind() in basicStringTypes:
            result=result+['<xju/Tagged.hh>','<string>']
            pass
        elif aliasOf.kind()==idltype.tk_sequence:
            if aliasOf.bound()==0:
                result=result+['<vector>']+tincludes(aliasOf)
            elif aliasOf.bound()==1:
                result=result+['<cxy/optional.hh>']+tincludes(aliasOf)
            else:
                result=result+['<cxy/BoundedSequence.hh>']+tincludes(aliasOf)
                pass
        else:
            result=tincludes(aliasOf)
            pass
        sizes=decl.declarators()[0].sizes()
        if len(sizes): result=result+['<xju/Array.hh>']
        pass
    elif isinstance(decl, idlast.Struct):
        for m in decl.members():
            if m.memberType().kind() in basicIntTypes:
                result=result+tincludes(m.memberType())
            elif m.memberType().kind() in basicFloatTypes:
                result=result+tincludes(m.memberType())
            elif m.memberType().kind() in basicStringTypes:
                result=result+['<string>']
                pass
            elif m.memberType().kind()==idltype.tk_sequence:
                if m.memberType().bound()==0:
                    result=result+['<vector>']+tincludes(m.memberType())
                elif m.memberType().bound()==1:
                    result=result+['<cxy/optional.hh>']+tincludes(m.memberType())
                else:
                    result=result+['<cxy/BoundedSequence.hh>']+tincludes(m.memberType())
                    pass
                pass
            pass
        pass
    elif isinstance(decl, idlast.Exception):
        for m in decl.members():
            if m.memberType().kind() in basicIntTypes:
                result=result+tincludes(m.memberType())
            elif m.memberType().kind() in basicFloatTypes:
                result=result+tincludes(m.memberType())
            elif m.memberType().kind() in basicStringTypes:
                result=result+['<string>']
                pass
            elif m.memberType().kind()==idltype.tk_sequence:
                if m.memberType().bound()==0:
                    result=result+['<vector>']+tincludes(m.memberType())
                elif m.memberType().bound()==1:
                    result=result+['<cxy/optional.hh>']+tincludes(m.memberType())
                else:
                    result=result+['<cxy/BoundedSequence.hh>']+tincludes(m.memberType())
                    pass
                pass
            pass
        pass
    elif isinstance(decl, idlast.Enum):
        result=['<sstream> // impl','<iostream>']
        pass
    elif isinstance(decl, idlast.Const):
        if decl.constType().kind() in basicIntTypes:
            result=result+tincludes(decl.constType())
        elif decl.constType().kind() in basicFloatTypes:
            result=result+tincludes(decl.constType())
        elif decl.constType().kind() in basicStringTypes:
            result=result+['<string>']
        else:
            result=[]
            pass
    elif isinstance(decl, idlast.Union):
        result=tincludes(decl.switchType())
        for c in decl.cases():
            assert c.constrType()==False,c
            for l in c.labels():
                if l.default() and \
                        not decl.switchType().kind()==idltype.tk_enum:
                    result.append('<xju/assert.hh>')
                else:
                    result.extend(tincludes(c.caseType()))
                pass
            pass
        pass
    elif isinstance(decl, idlast.Forward):
        result=[]
        pass
    elif isinstance(decl, idlast.StructForward):
        result=[]
        pass
    else:
        assert False, (str(decl.__class__),repr(decl))
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

def includeSpec(fileName,hpath,hhext):
    if os.path.dirname(fileName)=='':
        if hpath=='':
            return '"%s"'%(os.path.splitext(fileName)[0]+'.'+hhext)
        else:
            return '<%s%s>'%(hpath,os.path.splitext(fileName)[0]+'.'+hhext)
    return '<%s>'%(os.path.splitext(fileName)[0]+'.'+hhext)

def gen_idlincludes(fileNames,hpath,hhext):
    if not len(fileNames):
        return ''
    return '\n// included idl'+''.join(
        ['\n#include %s'%includeSpec(_,hpath,hhext) for _ in fileNames])

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
    hhext=([_.split('-hhext=',1)[1] for _ in args \
            if _.startswith('-hhext')]+\
           ['hh'])[0]
    if eheader.startswith('./') or os.path.dirname(eheader)=='':
        eheader='"%s"'%eheader[2:]
    else:
        eheader='<%s>'%eheader
    fileName=os.path.basename(tree.file())
    tincludes='\n'.join(['#include '+_ for _ in set(sum([gen_tincludes(_) for _ in tree.declarations() if _.mainFile()],[]))])
    hpath=([_.split('-hpath=',1)[1] for _ in args \
                if _.startswith('-hpath')]+\
               [''])[0]
    if len(hpath)>0 and not hpath.endswith('/'):
        hpath=hpath+'/'
    idlincludes=gen_idlincludes(set([_.file() for _ in tree.declarations() \
                                         if not _.mainFile()]),
                                hpath,
                                hhext)
    
    print head % vars()
    genResults=[gen(_,eclass,eheader,causeType,contextType)
                for _ in tree.declarations() if _.mainFile()]
    print '\n'.join([_.code for _ in genResults])
    print '\n'.join([_.genNamespaceScopeCode('') for _ in genResults])
    pass
