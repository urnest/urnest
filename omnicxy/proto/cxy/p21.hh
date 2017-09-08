#ifndef _OMNICXY_PROTO_P21_HH
#define _OMNICXY_PROTO_P21_HH
// generated from p21.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <cxy/BoundedSequence.hh>
#include <stdint.h>


class p21 
{
public:
  virtual ~p21() throw();
  struct X
  {
    
    cxy::BoundedSequence<int16_t,4> x_;
  
    X(
      cxy::BoundedSequence<int16_t,4> const& p1) throw();
    friend bool operator<(
      X const& x, 
      X const& y) throw() {
      if (x.x_<y.x_) return true;
      if (y.x_<x.x_) return false;
      return false;
    }
    friend bool operator>(
      X const& x, 
      X const& y) throw() {
      return y<x;
    }
    friend bool operator!=(
      X const& x, 
      X const& y) throw() {
      return (x<y)||(y<x);
    }
    friend bool operator==(
      X const& x, 
      X const& y) throw() {
      return !(x!=y);
    }
    friend bool operator<=(
      X const& x, 
      X const& y) throw() {
      return (x<y)||(x==y);
    }
    friend bool operator>=(
      X const& x, 
      X const& y) throw() {
      return (x>y)||(x==y);
    }
  };
  
  typedef cxy::BoundedSequence<int16_t,4> Short4;
  virtual ::p21::Short4 f(
    ::p21::Short4 const& x) throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
};


#endif
