#ifndef _OMNICXY_PROTO_P20_HH
#define _OMNICXY_PROTO_P20_HH
// generated from p20.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <cxy/Any.hh>
#include <stdint.h>
#include <vector>


namespace p20
{
struct A
{
  
  int16_t a_;

  A(
    int16_t const& p1) throw();
  friend bool operator<(
    A const& x, 
    A const& y) throw() {
    if (x.a_<y.a_) return true;
    if (y.a_<x.a_) return false;
    return false;
  }
  friend bool operator>(
    A const& x, 
    A const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    A const& x, 
    A const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    A const& x, 
    A const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    A const& x, 
    A const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    A const& x, 
    A const& y) throw() {
    return (x>y)||(x==y);
  }
};

typedef std::vector< int16_t > SS;
class F 
{
public:
  virtual ~F() throw();
  virtual ::cxy::Any< cxy::Exception > f1(
    ::cxy::Any< cxy::Exception > const& x) throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
  virtual int16_t f2(
    int16_t const& x) throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
};

}

#endif
