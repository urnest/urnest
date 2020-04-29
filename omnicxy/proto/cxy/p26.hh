#ifndef _OMNICXY_PROTO_P26_HH
#define _OMNICXY_PROTO_P26_HH
// generated from p26.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <tuple>
#include <stdint.h>
#include <string>


namespace p26
{
class I26 
{
public:
  virtual ~I26() throw();
  virtual int32_t e() /*throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception)*/ = 0;
  virtual int32_t f() /*throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception)*/ = 0;
  virtual std::tuple< std::string,int32_t > g() /*throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception)*/ = 0;
  virtual std::tuple< int32_t,std::string > h(
    int32_t const& x) /*throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception)*/ = 0;
};

}

#endif
