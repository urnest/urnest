#include "p2.sref.h"
#include "p2.h" // impl
#include "p2.cdr.h" // impl

#include <cxy/ORB.hh> // impl
#include <cxy/sref_impl.hh> // impl
#include <cxy/StubUserException.hh> // impl
#include <omniORB4/callDescriptor.h> // impl
#include <omniORB4/callHandle.h> // impl
#include <omniORB4/omniServant.h> // impl
#include <omniORB4/internal/giopStream.h> // impl

#include <xju/Optional.hh> //impl
#include <cxy/cdr.hh> //impl


// included idl
namespace
{
template<class T>
class calldesc
{
};
}
namespace cxy
{
}
namespace p2
{
}
namespace
{
template<>
class calldesc< ::p2::F >
{
public:

  class f1 : public omniCallDescriptor
  {
  public:
    f1(void (*lcfn)(omniCallDescriptor*, omniServant*),
       char const* op_,
       int op_len_,
       _CORBA_Boolean oneway,
       const char*const* user_excns_,
       int n_user_excns_,
       _CORBA_Boolean is_upcall_) throw():
        omniCallDescriptor(lcfn,
                           op_,op_len_,
                           oneway,
                           user_excns_,n_user_excns_,
                           is_upcall_)
    {
    }
    
    xju::Optional< int16_t > p1_;
    xju::Optional< int32_t > p2_;
    xju::Optional< double > p3_;
    xju::Optional< std::string > p4_;
    xju::Optional< uint8_t > p5_;
    xju::Optional< uint64_t > p6_;
    xju::Optional< bool > p7_;

    // omniCallDescriptor::
    void unmarshalArguments(cdrStream& s) throw(
      CORBA::SystemException,
      omni::giopStream::CommFailure
      )
    {
      p1_=cxy::cdr< int16_t >::unmarshalFrom(s);
      p2_=cxy::cdr< int32_t >::unmarshalFrom(s);
      p3_=cxy::cdr< double >::unmarshalFrom(s);
      p4_=cxy::cdr< std::string >::unmarshalFrom(s);
      p5_=cxy::cdr< uint8_t >::unmarshalFrom(s);
      p6_=cxy::cdr< uint64_t >::unmarshalFrom(s);
      p7_=cxy::cdr< bool >::unmarshalFrom(s);
    }
    // omniCallDescriptor::
    void marshalReturnedValues(cdrStream& s) throw(
      omni::giopStream::CommFailure
      )
    {
    }
  };
  class f2 : public omniCallDescriptor
  {
  public:
    f2(void (*lcfn)(omniCallDescriptor*, omniServant*),
       char const* op_,
       int op_len_,
       _CORBA_Boolean oneway,
       const char*const* user_excns_,
       int n_user_excns_,
       _CORBA_Boolean is_upcall_) throw():
        omniCallDescriptor(lcfn,
                           op_,op_len_,
                           oneway,
                           user_excns_,n_user_excns_,
                           is_upcall_)
    {
    }
    
    xju::Optional< int16_t > r_;

    // omniCallDescriptor::
    void unmarshalArguments(cdrStream& s) throw(
      CORBA::SystemException,
      omni::giopStream::CommFailure
      )
    {
    }
    // omniCallDescriptor::
    void marshalReturnedValues(cdrStream& s) throw(
      omni::giopStream::CommFailure
      )
    {
      cxy::cdr< int16_t >::marshal(r_.value(),s);
    }
  };
};
}

namespace cxy
{
sref< ::p2::F >::sref(cxy::ORB< cxy::Exception >& orb, 
       std::string const& name,
       ::p2::F& x) throw(
         cxy::Exceptions< cxy::Exception >::DuplicateName) try:
      name_(name),
      x_(x),
      c_(guard_),
      impl_deleted_(false),
      impl_(new cxy::sref_impl(orb.impl_, 
                               name, 
                               cxy::cdr< ::p2::F >::repoId, 
                               *this))
  {
  }
  catch(cxy::Exceptions<cxy::Exception>::DuplicateName& e)
  {
    if (typeid(cxy::Exception)==typeid(cxy::Exception)) {
      throw;
    }
    cxy::Exceptions< cxy::Exception >::DuplicateName ee(
      e.cause_.first, e.cause_.second);
    for (std::vector<std::pair<std::string, cxy::Exception::FileAndLine> >::const_iterator i=
           e.context_.begin();
         i != e.context_.end();
         ++i) {
      ee.addContext((*i).first, (*i).second);
    }
    throw ee;
  }
  
sref< ::p2::F >::~sref() throw()
  {
    xju::mt::Lock l(guard_);
    // impl might be deleted during deactivate or sometime after, which
    // is why we use a separate thread
    xju::mt::Thread<sref_impl> t(*impl_, &sref_impl::deactivate);
    while(!impl_deleted_) {
      c_.wait(l);
    }
  }

  
cxy::IOR< ::p2::F > sref< ::p2::F >::ior() const throw()
  {
     return cxy::IOR< ::p2::F >(impl_->ior());
  }

  
void sref< ::p2::F >::f1::lcfn(omniCallDescriptor* ocd, omniServant* svnt) 
      //exception spec commented to avoid header dependency on omniORB headers
      //throw(CORBA::UserException)
    {
      ::p2::F* impl = (::p2::F*) svnt->_ptrToInterface(cxy::cdr< ::p2::F >::repoId);
      calldesc< ::p2::F >::f1* c=(calldesc< ::p2::F >::f1*)ocd;
      if (!c->is_upcall()){
         impl->f1(
        c->p1_.value(),
        c->p2_.value(),
        c->p3_.value(),
        c->p4_.value(),
        c->p5_.value(),
        c->p6_.value(),
        c->p7_.value());
      }
      else {
        try {
           impl->f1(
        c->p1_.value(),
        c->p2_.value(),
        c->p3_.value(),
        c->p4_.value(),
        c->p5_.value(),
        c->p6_.value(),
        c->p7_.value());
        }
        catch(cxy::Exception const&) {
          throw; // REVISIT: support translation to a specified idl exception
        }
      }
    }
    
 const char* const sref< ::p2::F >::f1::_user_exns[] = {
      0
    };
  
void sref< ::p2::F >::f2::lcfn(omniCallDescriptor* ocd, omniServant* svnt) 
      //exception spec commented to avoid header dependency on omniORB headers
      //throw(CORBA::UserException)
    {
      ::p2::F* impl = (::p2::F*) svnt->_ptrToInterface(cxy::cdr< ::p2::F >::repoId);
      calldesc< ::p2::F >::f2* c=(calldesc< ::p2::F >::f2*)ocd;
      if (!c->is_upcall()){
        c->r_ = impl->f2();
      }
      else {
        try {
          c->r_ = impl->f2();
        }
        catch(cxy::Exception const&) {
          throw; // REVISIT: support translation to a specified idl exception
        }
      }
    }
    
 const char* const sref< ::p2::F >::f2::_user_exns[] = {
      0
    };
  
bool sref< ::p2::F >::_dispatch(omniCallHandle& _handle)
    //exception spec commented to avoid header dependency on omniORB headers
    // throw(CORBA::Exception)
  {
    const char* op = _handle.operation_name();
    
      
    if (omni::strMatch(op, "f1")) {
      calldesc< ::p2::F >::f1 c(f1::lcfn, "f1", 2+1, 0, f1::_user_exns, 0, 1);
      _handle.upcall(impl_, c);
      return 1;
    }
    
    if (omni::strMatch(op, "f2")) {
      calldesc< ::p2::F >::f2 c(f2::lcfn, "f2", 2+1, 0, f2::_user_exns, 0, 1);
      _handle.upcall(impl_, c);
      return 1;
    }
  
    return 0;
  }

  // sref_if::
  
void* sref< ::p2::F >::_ptrToInterface(const char* id) throw()
  {
    if (id == cxy::cdr< ::p2::F >::repoId) {
      return &x_;
    }
    if (id == ::CORBA::Object::_PD_repoId) {
      return (void*) 1;
    }
    if (omni::strMatch(id, cxy::cdr< ::p2::F >::repoId)) {
      return &x_;
    }
    if (omni::strMatch(id, ::CORBA::Object::_PD_repoId)) {
      return (void*) 1;
    }
    return 0;
  }
  
  // sref_if::
  
void sref< ::p2::F >::impl_deleted() throw()
  {
    xju::mt::Lock l(guard_);
    impl_deleted_=true;
    c_.signal(l);
  }

  
}
