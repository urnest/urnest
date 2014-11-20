from omniidl import idlast
from omniidl import idltype

import sys
import os.path

from cxy import ptype

objref_operation_t='''
// %(fqn)s::
void %(name)s(%(params)s) throw(
  %(eclass)s)
{
  try {
    calldesc::%(name)s c("%(name)s", 3, 0%(paramNames)s);
    _invoke(c);
  }
  catch(CORBA::Exception const& ee) {
    %(eclass)s e(cxy::translateException<%(eclass)s>(ee));
    e.addContext("%(fqn)s::%(name)s()", std::make_pair(__FILE__, __LINE__)); // REVISIT: add ior / id
    throw e;
  }
}
'''

calldesc_operation_t='''
class %(name)s : public omniCallDescriptor
{
public:
  %(name)s(const char* op_,
     size_t oplen,
     _CORBA_Boolean upcall%(params)s):
      omniCallDescriptor(
        &%(name)s::lcfn, op_, oplen, 0, _user_exns, 0, upcall)%(paramInits)s
  {
  }
  %(paramMembers)s
  // omniCallDescriptor::
  void marshalArguments(cdrStream& s)
  {
    %(paramMarshals)s
  }

  static void lcfn(omniCallDescriptor* calldesc, omniServant* svnt)
  {
    ::%(fqn)s* impl=(::%(fqn)s*)svnt->_ptrToInterface(cxy::cdr< ::%(fqn)s>::repoId);
    impl->%(name)s();
  }
  static const char* const _user_exns[] = {
    0
  };
};
'''

interface_t='''\
template<>
class objref< ::%(fqn)s>:
  public virtual ::%(fqn)s,
  public virtual ::CORBA::Object,
  public virtual omniObjRef,
  public xju::NonCopyable
{
public:
  inline objref()  { _PR_setobj(0); }  // nil
  objref(omniIOR* ior, omniIdentity* id) throw() :
      omniObjRef(cxy::cdr< ::%(fqn)s>::repoId, ior, id, 1) {
    _PR_setobj(this);
  }
  %(objref_content)s

protected:
  virtual ~objref() throw(){
  }
  
private:
  class calldesc
  {
  public:
    %(calldesc_content)s
  };
  // CORBA::Object::
  virtual void* _ptrToObjRef(const char* repoId)
  {
    if (repoId == cxy::cdr< ::%(fqn)s>::repoId)
      return this;
    
    if (omni::strMatch(repoId, cxy::cdr< ::%(fqn)s>::repoId))
      return this;
    
    if (repoId == ::CORBA::Object::_PD_repoId)
      return (::CORBA::Object_ptr) this;
    
    if (omni::strMatch(repoId, ::CORBA::Object::_PD_repoId))
      return (::CORBA::Object_ptr) this;
    
    return 0;
  }
};

template<>
class pof< ::%(fqn)s> : public omni::proxyObjectFactory {
public:
  pof() : omni::proxyObjectFactory(cxy::cdr< ::%(fqn)s>::repoId){
  }
  virtual ~pof()
  {
  }
  
  virtual omniObjRef* newObjRef(omniIOR* ior,omniIdentity* id)
  {
    return new cxy::objref< ::%(fqn)s>(ior, id);
  }
  
  virtual _CORBA_Boolean is_a(const char* repoId) const
  {
    if (repoId==cxy::cdr< ::%(fqn)s>::repoId ||
        omni::ptrStrMatch(repoId, cxy::cdr< ::%(fqn)s>::repoId)) {
      return 1;
    }
    return 0;
  }

  static pof< ::%(fqn)s>& me_() throw()
  {
    return me;
  }
  static pof< ::%(fqn)s> me;
};
'''

def reindent(indent, s):
    '''prepend %(indent)r to each line of %(s)r'''
    return '\n'.join([indent+_ for _ in s.split('\n')])

def genCalldesc(decl,eclass,eheader,indent,fqn):
    assert isinstance(decl, idlast.Operation),repr(decl)
    name=decl.identifier()
    pns=['p%s'%i for i in range(1, len(decl.parameters())+1)]
    params=','.join(['\n  %s& %s'%(ptype(p),n) for p,n in zip(decl.parameters(),pns)])
    paramInits=','.join(['\n    %s_(%s)'%(n,n) for n in pns])
    paramMembers=''.join(['\n  %s;'%pmembertype(p,n) for p,n in zip(decl.parameters(),pns)])
    paramMarshals=''.join(['\n  %s;'%pmarshal(p,n) for p,n in zip(decl.parameters(),pns)])
    assert not decl.oneway(), 'oneway not yet implemented'
    assert len(decl.raises())==0, 'raises not yet implemented'
    assert len(decl.contexts())==0, 'contexts not yet implemented'
    assert isinstance(decl.returnType(),idltype.Base) and decl.returnType().kind()==idltype.tk_void, 'returns not yet implemented'
    
    result=reindent(indent,calldesc_operation_t%vars())
    return result

def genObjref(decl,eclass,eheader,indent,fqn):
    assert isinstance(decl, idlast.Operation), repr(decl)
    name=decl.identifier()
    pns=['p%s'%i for i in range(1, len(decl.parameters())+1)]
    params=','.join(['\n  %s& %s'%(ptype(p),n) for p,n in zip(decl.parameters(),pns)])
    paramNames=','.join(['\n    %s'%n for n in pns])
    assert not decl.oneway(), 'oneway not yet implemented'
    assert len(decl.raises())==0, 'raises not yet implemented'
    assert len(decl.contexts())==0, 'contexts not yet implemented'
    assert isinstance(decl.returnType(),idltype.Base) and decl.returnType().kind()==idltype.tk_void, 'returns not yet implemented'
    
    result=reindent(indent,objref_operation_t%vars())
    return result

def gen(decl,eclass,eheader,indent=''):
    result=''
    if isinstance(decl, idlast.Module):
        result=''.join(gen(_,eclass,eheader,indent) for _ in decl.definitions())
    elif isinstance(decl, idlast.Interface):
        fqn='::'.join(decl.scopedName())
        repoId=decl.repoId()
        calldesc_content=''.join(
            [genCalldesc(_,eclass,eheader,indent+'    ',fqn) \
                 for _ in decl.contents()\
                 if isinstance(_,idlast.Operation)])
        objref_content=''.join(
            [genObjref(_,eclass,eheader,indent+'  ',fqn)\
                 for _ in decl.contents()\
                 if isinstance(_,idlast.Operation)])
        result=interface_t%vars()
    else:
        assert False, repr(decl)
        pass
    return result

template='''\
// generated from %(fileName)s by omnicxy cxyobjref idl backend specifying 
// %(eclass)s from %(eheader)s as base class for all ipc exceptions
#include "%(baseName)s.hh"
#include "%(baseName)s.cdr.hh"

#include <cxy/objref.hh>
#include <cxy/translateException.hh>
#include <cxy/pof.hh>

#include <xju/NonCopyable.hh>

#include <omniORB4/CORBA.h>
#include <omniORB4/callDescriptor.h>

namespace cxy
{

%(items)s
}
'''

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
    print template % vars()
    pass
