#ifndef _P2_SREF_H
#define _P2_SREF_H
// generated from p2.idl by omnicxy cxysref idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all exceptions
#include <cxy/sref.hh>
#include <cxy/sref_if.hh>
#include <cxy/Exceptions.hh>
#include <cxy/IOR.hh>
#include <cxy/Exception.hh>

#include <xju/mt.hh>
#include <string>
#include "p1.sref.h"

class omniCallHandle;
class omniCallDescriptor;
class omniServant;

namespace cxy
{
class sref_impl;

template<class BaseException>
class ORB;
}
namespace p2 {
class F;
}


namespace cxy
{
template<>
class sref< ::p2::F > :  private sref_if
{
public:
  // pre: lifetime(x) includes lifetime(this)
  // pre: lifetime(orb) includes lifetime(this)
  sref(cxy::ORB< cxy::Exception >& orb, 
       std::string const& name,
       ::p2::F& x) throw(
         cxy::Exceptions< cxy::Exception >::DuplicateName);

  ~sref() throw();


  cxy::IOR< ::p2::F > ior() const throw();


  std::string const name_;
  ::p2::F& x_;
  
private:
    
  class f1 
  {
  public:
    static void lcfn(omniCallDescriptor* ocd, omniServant* svnt);

    static const char* const _user_exns[] ;
};
    
  class f2 
  {
  public:
    static void lcfn(omniCallDescriptor* ocd, omniServant* svnt);

    static const char* const _user_exns[] ;
};
  

  // sref_if::
  virtual bool _dispatch(omniCallHandle& _handle);


  // sref_if::
  virtual void* _ptrToInterface(const char* id) throw();

  
  // sref_if::
  virtual void impl_deleted() throw();


  xju::mt::Mutex guard_;
  xju::mt::Condition c_;
  bool impl_deleted_;
  cxy::sref_impl* impl_;
  
};
}

#endif
