from omniidl import idlast
from omniidl import idltype

import sys
import os.path

calldesc_operation_t='''
class %(name)s : public omniCallDescriptor
{
public:
  static void lcfn(omniCallDescriptor* calldesc, omniServant* svnt)
  {
    ::%(fqn)s* impl=(::%(fqn)s*)svnt->_ptrToInterface(cxy::cdr< ::%(fqn)s>::repoId);
    impl->%(name)s();
  }
  
  %(name)s(const char* op_,
     size_t oplen,
     _CORBA_Boolean upcall):
      omniCallDescriptor(
        &%(name)s::lcfn, op_, oplen, 0, _user_exns, 0, upcall)
  {
  }
  static const char* const _user_exns[] = {
    0
  };
};
'''

objref_operation_t='''
// %(fqn)s::
void %(name)s() throw(
  cxy::Exception)
{
  try {
    cxy::calldesc< ::%(fqn)s>::%(name)s c("%(name)s", 3, 0);
    _invoke(c);
  }
  catch(CORBA::Exception const& ee) {
    cxy::Exception e(cxy::translateException<cxy::Exception>(ee));
    e.addContext("%(fqn)s::%(name)s()", std::make_pair(__FILE__, __LINE__)); // REVISIT: add ior / id
    throw e;
  }
}
'''

interface_t='''\
template<>
class calldesc< ::%(fqn)s>
{
public:
  %(calldesc_content)s
};

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

def genCalldesc(decl,indent,fqn):
    result=''
    if isinstance(decl, idlast.Operation):
        name=decl.identifier()
        assert not decl.oneway(), 'oneway not yet implemented'
        assert len(decl.parameters())==0, 'parameters not yet implemented'
        assert len(decl.raises())==0, 'raises not yet implemented'
        assert len(decl.contexts())==0, 'contexts not yet implemented'
        assert isinstance(decl.returnType(),idltype.Base) and decl.returnType().kind()==idltype.tk_void, 'returns not yet implemented'
        
        result=reindent(indent,calldesc_operation_t%vars())
    else:
        assert False, repr(decl)
        pass
    return result

def genObjref(decl,indent,fqn):
    result=''
    if isinstance(decl, idlast.Operation):
        name=decl.identifier()
        assert not decl.oneway(), 'oneway not yet implemented'
        assert len(decl.parameters())==0, 'parameters not yet implemented'
        assert len(decl.raises())==0, 'raises not yet implemented'
        assert len(decl.contexts())==0, 'contexts not yet implemented'
        assert isinstance(decl.returnType(),idltype.Base) and decl.returnType().kind()==idltype.tk_void, 'returns not yet implemented'
        
        result=reindent(indent,objref_operation_t%vars())
    else:
        assert False, repr(decl)
        pass
    return result

def gen(decl,indent=''):
    result=''
    if isinstance(decl, idlast.Module):
        result=''.join(gen(_) for _ in decl.definitions())
    elif isinstance(decl, idlast.Interface):
        fqn='::'.join(decl.scopedName())
        repoId=decl.repoId()
        calldesc_content=''.join([genCalldesc(_,indent+'  ',fqn) for _ in decl.contents()])
        objref_content=''.join([genObjref(_,indent+'  ',fqn) for _ in decl.contents()])
        result=interface_t%vars()
    else:
        assert False, repr(decl)
        pass
    return result

template='''\
// generated from %(fileName)s by omnicxy cxyobjref idl backend specifying 
// cxy::Exception from "cxy/Exception.hh" as base class for all ipc exceptions
#include "%(baseName)s.hh"
#include "%(baseName)s.cdr.hh"

#include <cxy/objref.hh>
#include <cxy/calldesc.hh>
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
    assert tree.file().endswith('.idl'), tree.file()
    fileName=os.path.basename(tree.file())
    baseName=fileName[0:-4]
    items=''.join([gen(_) for _ in tree.declarations() if _.mainFile()])
    print template % vars()
    pass
