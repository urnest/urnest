#ifndef _OMNICXY_PROTO_P16_HH
#define _OMNICXY_PROTO_P16_HH
// generated from p16.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <cxy/IOR.hh>
#include <stdint.h>


namespace p16
{
class AAA 
{
public:
  virtual ~AAA() throw();
  virtual int32_t f(
    int32_t const& x) /*throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception)*/ = 0;
};

class BBB 
{
public:
  virtual ~BBB() throw();
  virtual ::cxy::IOR< ::p16::AAA > getA() /*throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception)*/ = 0;
};

class CCC :
  public virtual p16::AAA
{
public:
  virtual ~CCC() throw();
  virtual int32_t f2(
    int32_t const& x) /*throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception)*/ = 0;
};

}

#endif
