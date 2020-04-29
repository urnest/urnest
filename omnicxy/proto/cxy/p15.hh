#ifndef _OMNICXY_PROTO_P15_HH
#define _OMNICXY_PROTO_P15_HH
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
    int32_t const& x) /*throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception)*/ = 0;
};

class BBB 
{
public:
  virtual ~BBB() throw();
  virtual ::cxy::IOR< ::p15::AAA > getA() /*throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception)*/ = 0;
  virtual ::cxy::IOR< ::p15::AAA > echoA(
    ::cxy::IOR< ::p15::AAA > const& a) /*throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception)*/ = 0;
};

struct CCC
{
  
  ::cxy::IOR< ::p15::AAA > a_;

  CCC(
    ::cxy::IOR< ::p15::AAA > const& p1) throw();
  friend bool operator<(
    CCC const& x, 
    CCC const& y) throw() {
    if (x.a_<y.a_) return true;
    if (y.a_<x.a_) return false;
    return false;
  }
  friend bool operator>(
    CCC const& x, 
    CCC const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    CCC const& x, 
    CCC const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    CCC const& x, 
    CCC const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    CCC const& x, 
    CCC const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    CCC const& x, 
    CCC const& y) throw() {
    return (x>y)||(x==y);
  }
};

}

#endif
