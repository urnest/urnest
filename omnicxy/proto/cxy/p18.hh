#ifndef _P18_HH
#define _P18_HH
// generated from p18.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <cxy/IOR.hh>
#include <stdint.h>


namespace p18
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

  virtual ::cxy::IOR< void > get() throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
};

}
#endif
