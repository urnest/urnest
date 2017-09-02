#ifndef _P5_HH
#define _P5_HH
// generated from p5.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <cxy/optional.hh>
#include <stdint.h>
#include <vector>

// included idl
#include "p4.hh"
#include <omnicxy/proto/p3.hh>

namespace p5
{
typedef std::vector< ::p4::XS1 > JW2s;
class F 
{
public:
  virtual ~F() throw();
  virtual ::p5::JW2s f1(
    ::p5::JW2s const& p1) throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
};

typedef std::vector< uint32_t > LS;
typedef cxy::optional< bool > LT;
struct X
{
  
  cxy::optional< bool > y;
  ::p5::LT z;

  X(
    cxy::optional< bool > const& p1,
    ::p5::LT const& p2) throw();
  friend bool operator<(
    X const& x, 
    X const& y) throw() {
    if (x.y<y.y) return true;
    if (y.y<x.y) return false;
    if (x.z<y.z) return true;
    if (y.z<x.z) return false;
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

}

#endif
