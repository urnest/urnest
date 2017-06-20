#ifndef _P2_CREF_H
#define _P2_CREF_H
// generated from p2.idl by omnicxy cxycref idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all exceptions
#include <cxy/cref.hh>
#include <cxy/Exceptions.hh>
#include <cxy/Exception.hh>

#include "p2.h"

#include <string>
#include <xju/check_types_related.hh>

namespace cxy
{
template<class E>
class ORB;

template<class T>
class objref;

template<class T>
class IOR;

template<>
class cref< ::p2::F >
{
public:
  explicit cref(cxy::ORB< cxy::Exception >& orb, 
                std::string const& uri) throw(
    // no object with specified uri, including server
    // not reachable and server does not know name
    cxy::Exceptions< cxy::Exception >::NoSuchObject,
    // object with specified uri is not a p2::F
    cxy::Exceptions< cxy::Exception >::WrongType,
    // other failure, eg communication failure
    cxy::Exception);


  explicit cref(cxy::ORB< cxy::Exception >& orb, 
                cxy::IOR< ::p2::F >const& ior) throw(
    // no object with specified ior, including server
    // not reachable and server does not know name
    cxy::Exceptions< cxy::Exception >::NoSuchObject,
    // other failure, eg communication failure
    cxy::Exception);


  // note T must be a p2::F or p2::F must be a T
  template<class T>
  explicit cref(cxy::ORB< cxy::Exception >& orb, 
                cxy::IOR< T > const& ior) throw(
    // no object with specified ior, including server
    // not reachable and server does not know name
    cxy::Exceptions< cxy::Exception >::NoSuchObject,
    // object with specified uri is not a p2::F
    cxy::Exceptions< cxy::Exception >::WrongType,
    // other failure, eg communication failure
    cxy::Exception):
      orb_(&orb),
      obj_(locate(orb, ior.toString()))
  {
    xju::check_types_related<T, p2::F>();
  }

  cref(cref const& b) throw();


  cref< ::p2::F >& operator=(cref< ::p2::F > const& b) throw();


  // pre: T is a p2::F 
  template<class T>
  cref<T> narrow() const throw(
    // referenced server object is not a T
    cxy::Exceptions< cxy::Exception >::WrongType) {
    p2::F const* T_must_be_a_((T const*)0);
    return cref<T>(*orb_, uri());
  }

  cxy::IOR< p2::F > ior() const throw();


  operator cxy::IOR< p2::F >() const throw();


  ~cref() throw();

  
  p2::F* operator->() throw();

  p2::F const* operator->() const throw();

  p2::F& operator*() throw();

  p2::F const& operator*() const throw();

  
private:
  cxy::ORB< cxy::Exception >* orb_;
  cxy::objref< ::p2::F >* obj_;

  std::string uri() const throw();


  static cxy::objref< ::p2::F >* locate(
    cxy::ORB< cxy::Exception >& orb,
    std::string ior) throw(
      // no object with specified ior, including server
      // not reachable and server does not know name
      cxy::Exceptions< cxy::Exception >::NoSuchObject,
      // object with specified uri is not a p2::F
      cxy::Exceptions< cxy::Exception >::WrongType,
      // other failure, eg communication failure
      cxy::Exception);

};

}

#endif
