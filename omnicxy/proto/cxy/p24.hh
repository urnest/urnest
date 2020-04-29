#ifndef _OMNICXY_PROTO_P24_HH
#define _OMNICXY_PROTO_P24_HH
// generated from p24.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <cxy/TypeCode.hh>


namespace p24
{
class i24 
{
public:
  virtual ~i24() throw();
  virtual ::cxy::TypeCode f(
    ::cxy::TypeCode const& x) /*throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception)*/ = 0;
  struct Y
  {
    
    ::cxy::TypeCode y_;
  
    Y(
      ::cxy::TypeCode const& p1) throw();
    friend bool operator<(
      Y const& x, 
      Y const& y) throw() {
      if (x.y_<y.y_) return true;
      if (y.y_<x.y_) return false;
      return false;
    }
    friend bool operator>(
      Y const& x, 
      Y const& y) throw() {
      return y<x;
    }
    friend bool operator!=(
      Y const& x, 
      Y const& y) throw() {
      return (x<y)||(y<x);
    }
    friend bool operator==(
      Y const& x, 
      Y const& y) throw() {
      return !(x!=y);
    }
    friend bool operator<=(
      Y const& x, 
      Y const& y) throw() {
      return (x<y)||(x==y);
    }
    friend bool operator>=(
      Y const& x, 
      Y const& y) throw() {
      return (x>y)||(x==y);
    }
  };
  
};

}

#endif
