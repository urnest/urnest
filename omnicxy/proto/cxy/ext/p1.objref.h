#ifndef _P1_OBJREF_H
#define _P1_OBJREF_H
// generated from p1.idl by omnicxy cxyobjref idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include "p1.h"
#include "p1.cdr.h"

#include <cxy/objref.hh>
#include <cxy/pof.hh>
#include <xju/NonCopyable.hh>
#include <xju/Optional.hh>

#include <omniORB4/CORBA.h>
#include <omniORB4/callDescriptor.h>
#include <omniORB4/IOP_C.h>


namespace cxy
{

template<>
class objref< ::p1::F >:
  public virtual ::p1::F,
  public virtual ::CORBA::Object,
  public virtual omniObjRef,
  public virtual xju::NonCopyable
{
public:
  objref();  // nil
  objref(omniIOR* ior, omniIdentity* id) throw();
  std::string uri_;

  IORImpl ior(std::string const& repoId) throw();

    
  // p1::F::
  void f1() throw(
    cxy::Exception);
  

protected:
  virtual ~objref() throw();
  
private:
  class calldesc
  {
  public:
        
    class f1 : public omniCallDescriptor
    {
    public:
      f1(
        const char* op_,
        size_t oplen,
        _CORBA_Boolean upcall) throw();
      
    
      // omniCallDescriptor::
      void marshalArguments(cdrStream& s) // REVISIT: throw
      ;
      void unmarshalReturnedValues(cdrStream& s) // REVISIT: throw
      ;
      void userException(cdrStream& s,
                         omni::IOP_C* iop_client,
                         const char* repoId)//REVISIT: throw
      ;
    
      static void lcfn(omniCallDescriptor* calldesc, omniServant* svnt)
      ;
      static const char* const _user_exns[] ;
};
    
  };
  // CORBA::Object::
  virtual void* _ptrToObjRef(const char* repoId)
  ;
};

template<>
class pof< ::p1::F > : public omni::proxyObjectFactory {
public:
  pof() ;
  virtual ~pof()
  ;
  
  virtual omniObjRef* newObjRef(omniIOR* ior,omniIdentity* id)
  ;
  
  virtual _CORBA_Boolean is_a(const char* repoId) const;

  static pof< ::p1::F >& me_() throw();
  static pof< ::p1::F > me;

};

}

#endif
