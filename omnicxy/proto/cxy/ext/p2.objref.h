#ifndef _P2_OBJREF_H
#define _P2_OBJREF_H
// generated from p2.idl by omnicxy cxyobjref idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include "p2.h"
#include "p2.cdr.h"

#include <cxy/objref.hh>
#include <cxy/pof.hh>
#include <xju/NonCopyable.hh>
#include <xju/Optional.hh>

#include <omniORB4/CORBA.h>
#include <omniORB4/callDescriptor.h>
#include <omniORB4/IOP_C.h>

// included idl
#include "p1.objref.h"

namespace cxy
{

template<>
class objref< ::p2::F >:
  public virtual ::p2::F,
  public virtual ::CORBA::Object,
  public virtual omniObjRef,
  public virtual xju::NonCopyable
{
public:
  objref();  // nil
  objref(omniIOR* ior, omniIdentity* id) throw();
  std::string uri_;

  IORImpl ior(std::string const& repoId) throw();

    
  // p2::F::
  void f1(
    int16_t const& p1,
    int32_t const& p2,
    double const& p3,
    std::string const& p4,
    uint8_t const& p5,
    uint64_t const& p6,
    bool const& p7) throw(
    cxy::Exception);
    
  // p2::F::
  int16_t f2() throw(
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
        _CORBA_Boolean upcall,
        int16_t const& p1,
        int32_t const& p2,
        double const& p3,
        std::string const& p4,
        uint8_t const& p5,
        uint64_t const& p6,
        bool const& p7) throw();
      
      int16_t const p1_;
      int32_t const p2_;
      double const p3_;
      std::string const p4_;
      uint8_t const p5_;
      uint64_t const p6_;
      bool const p7_;
    
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
        
    class f2 : public omniCallDescriptor
    {
    public:
      f2(
        const char* op_,
        size_t oplen,
        _CORBA_Boolean upcall) throw();
      
      xju::Optional< int16_t > r_;
    
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
class pof< ::p2::F > : public omni::proxyObjectFactory {
public:
  pof() ;
  virtual ~pof()
  ;
  
  virtual omniObjRef* newObjRef(omniIOR* ior,omniIdentity* id)
  ;
  
  virtual _CORBA_Boolean is_a(const char* repoId) const;

  static pof< ::p2::F >& me_() throw();
  static pof< ::p2::F > me;

};

}

#endif
