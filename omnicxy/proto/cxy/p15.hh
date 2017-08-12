#ifndef _P15_HH
#define _P15_HH
// generated from p15.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <cxy/IOR.hh>
#include <stdint.h>


namespace p15
{
class AAA 
{
public:
  virtual ~AAA() throw();

  virtual int32_t f(
    int32_t const& x) throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
};

class BBB 
{
public:
  virtual ~BBB() throw();

  virtual ::cxy::IOR< ::p15::AAA > getA() throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
  virtual ::cxy::IOR< ::p15::AAA > echoA(
    ::cxy::IOR< ::p15::AAA > const& a) throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
};

}

#endif
