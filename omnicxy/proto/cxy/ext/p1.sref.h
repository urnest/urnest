#ifndef _P1_SREF_H
#define _P1_SREF_H
// generated from p1.idl by omnicxy cxysref idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all exceptions
#include <cxy/sref.hh>
#include <cxy/sref_if.hh>
#include <cxy/Exceptions.hh>
#include <cxy/IOR.hh>
#include <cxy/Exception.hh>

#include <xju/mt.hh>
#include <string>
class omniCallHandle;
class omniCallDescriptor;
class omniServant;

namespace cxy
{
class sref_impl;

template<class BaseException>
class ORB;
}
namespace p1 {
class F;
}


namespace cxy
{
template<>
class sref< ::p1::F > :  private sref_if
{
public:
  // pre: lifetime(x) includes lifetime(this)
  // pre: lifetime(orb) includes lifetime(this)
  sref(cxy::ORB< cxy::Exception >& orb, 
       std::string const& name,
       ::p1::F& x) throw(
         cxy::Exceptions< cxy::Exception >::DuplicateName);
  ~sref() throw();

  cxy::IOR< ::p1::F > ior() const throw();

  std::string const name_;
  ::p1::F& x_;
  
private:
    
  class f1 
  {
  public:
    static void lcfn(omniCallDescriptor* ocd, omniServant* svnt) 
      //exception spec commented to avoid header dependency on omniORB headers
      //throw(CORBA::UserException)
    ;
    static const char* const _user_exns[] ;
};
  

  // sref_if::
  virtual bool _dispatch(omniCallHandle& _handle)
    //exception spec commented to avoid header dependency on omniORB headers
    // throw(CORBA::Exception)
  ;

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
