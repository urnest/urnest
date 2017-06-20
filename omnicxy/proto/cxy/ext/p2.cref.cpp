#include "p2.cref.h"
#include "p2.objref.h" // impl
#include "p2.cdr.h" // impl

#include <cxy/ORB.hh> // impl
#include <cxy/translateException.hh> // impl
#include <cxy/IOR.hh> // impl
#include <xju/format.hh> // impl
#include <xju/assert.hh> // impl
namespace cxy
{
cref< ::p2::F >::cref(cxy::ORB< cxy::Exception >& orb, 
                std::string const& uri) throw(
    // no object with specified uri, including server
    // not reachable and server does not know name
    cxy::Exceptions< cxy::Exception >::NoSuchObject,
    // object with specified uri is not a p2::F
    cxy::Exceptions< cxy::Exception >::WrongType,
    // other failure, eg communication failure
    cxy::Exception):
      orb_(&orb),
      obj_((&cxy::pof< ::p2::F >::me_(), // force init of static var,
            (cxy::objref< ::p2::F >*)orb.locate(
              uri, cxy::cdr< ::p2::F >::repoId)))
  {
    obj_->uri_=uri;
  }

  
cref< ::p2::F >::cref(cxy::ORB< cxy::Exception >& orb, 
                cxy::IOR< ::p2::F >const& ior) throw(
    // no object with specified ior, including server
    // not reachable and server does not know name
    cxy::Exceptions< cxy::Exception >::NoSuchObject,
    // other failure, eg communication failure
    cxy::Exception):
      orb_(&orb),
      obj_((&cxy::pof< ::p2::F >::me_(), // force init of static var,
            (cxy::objref< ::p2::F >*)orb.locate(
              ior.toString(), cxy::cdr< ::p2::F >::repoId)))
  {
    obj_->uri_=ior.toString();
  }

  // note T must be a p2::F or p2::F must be a T
  
cref< ::p2::F >::cref(cref const& b) throw():
      orb_(b.orb_),
      obj_(b.obj_)
  {
    if (obj_ && !obj_->_NP_is_nil())  omni::duplicateObjRef(obj_);
  }

  
cref< ::p2::F >& cref< ::p2::F >::operator=(cref< ::p2::F > const& b) throw()
  {
    if (this != &b) {
      ::CORBA::release(obj_);
      orb_ = b.orb_;
      obj_ = b.obj_;
      if (obj_ && !obj_->_NP_is_nil())  omni::duplicateObjRef(obj_);
    }
    return *this;
  }

  // pre: T is a p2::F 
  
cxy::IOR< p2::F > cref< ::p2::F >::ior() const throw(){
    return cxy::IOR< p2::F >(obj_->ior(cxy::cdr< ::p2::F >::repoId));
  }

  
cref< ::p2::F >::operator cxy::IOR< p2::F >() const throw(){
    return ior();
  }

  
cref< ::p2::F >::~cref() throw()
  {
    ::CORBA::release(obj_);
  }
  
  
p2::F* cref< ::p2::F >::operator->() throw()
  {
    return obj_;
  }
  
p2::F const* cref< ::p2::F >::operator->() const throw()
  {
    return obj_;
  }
  
p2::F& cref< ::p2::F >::operator*() throw()
  {
    return *obj_;
  }
  
p2::F const& cref< ::p2::F >::operator*() const throw()
  {
    return *obj_;
  }
  

std::string cref< ::p2::F >::uri() const throw(){
    return obj_->uri_;
  }

  
cxy::objref< ::p2::F >* cref< ::p2::F >::locate(
    cxy::ORB< cxy::Exception >& orb,
    std::string ior) throw(
      // no object with specified ior, including server
      // not reachable and server does not know name
      cxy::Exceptions< cxy::Exception >::NoSuchObject,
      // object with specified uri is not a p2::F
      cxy::Exceptions< cxy::Exception >::WrongType,
      // other failure, eg communication failure
      cxy::Exception) {
    cxy::pof< ::p2::F >::me_(); // force init of static var
    cxy::objref< ::p2::F >* result(
      (cxy::objref< ::p2::F >*)orb.locate(
        ior, cxy::cdr< ::p2::F >::repoId));
    result->uri_=ior;
    return result;
  }

}
