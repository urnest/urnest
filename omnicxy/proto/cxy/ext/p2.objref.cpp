#include "p2.objref.h"
#include <cxy/translateException.hh> //impl
#include <cxy/translateCorbaException.hh> //impl
#include <cxy/TaggedProfileList.hh>//impl

namespace cxy
{
objref< ::p2::F >::objref(): uri_("nil") { _PR_setobj(0); }  // nil
  
objref< ::p2::F >::objref(omniIOR* ior, omniIdentity* id) throw() :
      omniObjRef(cxy::cdr< ::p2::F >::repoId, ior, id, 1) {
    _PR_setobj(this);
  }
  
IORImpl objref< ::p2::F >::ior(std::string const& repoId) throw() {
    omniIOR_var const ior(_getIOR());
    return IORImpl(repoId,
                   cxy::TaggedProfileList(ior->iopProfiles()));
  }

    
  // p2::F::
  
void objref< ::p2::F >::f1(
    int16_t const& p1,
    int32_t const& p2,
    double const& p3,
    std::string const& p4,
    uint8_t const& p5,
    uint64_t const& p6,
    bool const& p7) throw(
    cxy::Exception)
  {
    try {
      try {
        calldesc::f1 c("f1", 2+1, 0,
        p1,
        p2,
        p3,
        p4,
        p5,
        p6,
        p7);
        _invoke(c);
      }
      catch(CORBA::Exception const& ee) {
        throw cxy::translateException< cxy::Exception >(
          translateCorbaException(ee));
      }
    }
    catch(cxy::Exception& e) {
      e.addContext(uri_+"->p2::F::f1()", std::make_pair(__FILE__, __LINE__));
      throw;
    }
  }
    
  // p2::F::
  
int16_t objref< ::p2::F >::f2() throw(
    cxy::Exception)
  {
    try {
      try {
        calldesc::f2 c("f2", 2+1, 0);
        _invoke(c);
      return c.r_.value();
      }
      catch(CORBA::Exception const& ee) {
        throw cxy::translateException< cxy::Exception >(
          translateCorbaException(ee));
      }
    }
    catch(cxy::Exception& e) {
      e.addContext(uri_+"->p2::F::f2()", std::make_pair(__FILE__, __LINE__));
      throw;
    }
  }
  


objref< ::p2::F >::~objref() throw(){
  }
  

objref< ::p2::F >::calldesc::f1::f1(
        const char* op_,
        size_t oplen,
        _CORBA_Boolean upcall,
        int16_t const& p1,
        int32_t const& p2,
        double const& p3,
        std::string const& p4,
        uint8_t const& p5,
        uint64_t const& p6,
        bool const& p7) throw():
          omniCallDescriptor(
            &f1::lcfn, op_, oplen, 0, _user_exns, 0, upcall),
          p1_(p1),
          p2_(p2),
          p3_(p3),
          p4_(p4),
          p5_(p5),
          p6_(p6),
          p7_(p7)
      {
      }
      
      
void objref< ::p2::F >::calldesc::f1::marshalArguments(cdrStream& s) // REVISIT: throw
      {
        cxy::cdr< int16_t >::marshal(p1_, s);
        cxy::cdr< int32_t >::marshal(p2_, s);
        cxy::cdr< double >::marshal(p3_, s);
        cxy::cdr< std::string >::marshal(p4_, s);
        cxy::cdr< uint8_t >::marshal(p5_, s);
        cxy::cdr< uint64_t >::marshal(p6_, s);
        cxy::cdr< bool >::marshal(p7_, s);
      }
      
void objref< ::p2::F >::calldesc::f1::unmarshalReturnedValues(cdrStream& s) // REVISIT: throw
      {
      }
      
void objref< ::p2::F >::calldesc::f1::userException(cdrStream& s,
                         omni::IOP_C* iop_client,
                         const char* repoId)//REVISIT: throw
      {
        if (iop_client) iop_client->RequestCompleted(1);
        OMNIORB_THROW(UNKNOWN,omni::UNKNOWN_UserException,
                      (::CORBA::CompletionStatus)s.completion());
      }
    
      
void objref< ::p2::F >::calldesc::f1::lcfn(omniCallDescriptor* calldesc, omniServant* svnt)
      {
        ::p2::F* impl=(::p2::F*)svnt->_ptrToInterface(cxy::cdr< ::p2::F >::repoId);
        f1* cd((f1*)calldesc);
         impl->f1(
          cd->p1_,
          cd->p2_,
          cd->p3_,
          cd->p4_,
          cd->p5_,
          cd->p6_,
          cd->p7_);
      }
      
 const char* const objref< ::p2::F >::calldesc::f1::_user_exns[] = {
        0
      };
    
objref< ::p2::F >::calldesc::f2::f2(
        const char* op_,
        size_t oplen,
        _CORBA_Boolean upcall) throw():
          omniCallDescriptor(
            &f2::lcfn, op_, oplen, 0, _user_exns, 0, upcall)
      {
      }
      
      
void objref< ::p2::F >::calldesc::f2::marshalArguments(cdrStream& s) // REVISIT: throw
      {
      }
      
void objref< ::p2::F >::calldesc::f2::unmarshalReturnedValues(cdrStream& s) // REVISIT: throw
      {
        r_=cxy::cdr< int16_t >::unmarshalFrom(s);
      }
      
void objref< ::p2::F >::calldesc::f2::userException(cdrStream& s,
                         omni::IOP_C* iop_client,
                         const char* repoId)//REVISIT: throw
      {
        if (iop_client) iop_client->RequestCompleted(1);
        OMNIORB_THROW(UNKNOWN,omni::UNKNOWN_UserException,
                      (::CORBA::CompletionStatus)s.completion());
      }
    
      
void objref< ::p2::F >::calldesc::f2::lcfn(omniCallDescriptor* calldesc, omniServant* svnt)
      {
        ::p2::F* impl=(::p2::F*)svnt->_ptrToInterface(cxy::cdr< ::p2::F >::repoId);
        f2* cd((f2*)calldesc);
        
        cd->r_= impl->f2();
      }
      
 const char* const objref< ::p2::F >::calldesc::f2::_user_exns[] = {
        0
      };
    
void* objref< ::p2::F >::_ptrToObjRef(const char* repoId)
  {
    if (repoId == cxy::cdr< ::p2::F >::repoId)
      return this;
    
    if (omni::strMatch(repoId, cxy::cdr< ::p2::F >::repoId))
      return this;
    
    if (repoId == ::CORBA::Object::_PD_repoId)
      return (::CORBA::Object_ptr) this;
    
    if (omni::strMatch(repoId, ::CORBA::Object::_PD_repoId))
      return (::CORBA::Object_ptr) this;
    
    return 0;
  }

pof< ::p2::F >::pof() : omni::proxyObjectFactory(cxy::cdr< ::p2::F >::repoId){
  }
  
pof< ::p2::F >::~pof()
  {
  }
  
  
omniObjRef* pof< ::p2::F >::newObjRef(omniIOR* ior,omniIdentity* id)
  {
    return new cxy::objref< ::p2::F >(ior, id);
  }
  
  
_CORBA_Boolean pof< ::p2::F >::is_a(const char* repoId) const
  {
    if (repoId==cxy::cdr< ::p2::F >::repoId) {
      return 1;
    }
    if (omni::ptrStrMatch(repoId, cxy::cdr< ::p2::F >::repoId)) {
      return 1;
    }
    return 0;
  }

  
pof< ::p2::F >& pof< ::p2::F >::me_() throw()
  {
    return me;
  }
  
 pof< ::p2::F > pof< ::p2::F >::me;

}
