#ifndef _OMNICXY_PROTO_P25_HH
#define _OMNICXY_PROTO_P25_HH
// generated from p25.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <stdint.h>
#include <xju/Array.hh>


namespace p25
{
struct Two
{
  
  int32_t a_;
  int32_t b_;

  Two(
    int32_t const& p1,
    int32_t const& p2) throw();
  friend bool operator<(
    Two const& x, 
    Two const& y) throw() {
    if (x.a_<y.a_) return true;
    if (y.a_<x.a_) return false;
    if (x.b_<y.b_) return true;
    if (y.b_<x.b_) return false;
    return false;
  }
  friend bool operator>(
    Two const& x, 
    Two const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    Two const& x, 
    Two const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    Two const& x, 
    Two const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    Two const& x, 
    Two const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    Two const& x, 
    Two const& y) throw() {
    return (x>y)||(x==y);
  }
};

typedef xju::Array< ::p25::Two,3,cxy::Exception > ThreeTwos;
class I25 
{
public:
  virtual ~I25() throw();
  virtual ::p25::ThreeTwos f25(
    ::p25::ThreeTwos const& x) /*throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception)*/ = 0;
};

}

#endif
