#ifndef _P4_HH
#define _P4_HH
// generated from p4.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <string>
#include <stdint.h>

// included idl
#include <omnicxy/proto/p3.hh>

namespace p4
{
struct XS1
{
  
  ::p3::MyInt a_;
  std::string b_;

  XS1(
    ::p3::MyInt const& p1,
    std::string const& p2) throw();

  friend bool operator<(
    XS1 const& x, 
    XS1 const& y) throw() {
    if (x.a_<y.a_) return true;
    if (y.a_<x.a_) return false;
    if (x.b_<y.b_) return true;
    if (y.b_<x.b_) return false;
    return false;
  }
  friend bool operator>(
    XS1 const& x, 
    XS1 const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    XS1 const& x, 
    XS1 const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    XS1 const& x, 
    XS1 const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    XS1 const& x, 
    XS1 const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    XS1 const& x, 
    XS1 const& y) throw() {
    return (x>y)||(x==y);
  }
};

class F 
{
public:
  virtual ~F() throw();

  virtual ::p4::XS1 f1(
    ::p4::XS1 const& y) throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
  struct XxPair : public std::pair< int32_t, std::string >
  {
    XxPair(int32_t const& p1, std::string const& p2) throw();

    template<class T1, class T2>
    explicit XxPair(std::pair<T1, T2> const& x) throw():
       std::pair< int32_t, std::string >(x.first,std::string(x.second)) {
    }
    friend bool operator<(
      XxPair const& x, 
      XxPair const& y) throw() {
      if (x.first < y.first) return true;
      if (y.first < x.first) return false;
      return false;
    }
    friend bool operator>(
      XxPair const& x, 
      XxPair const& y) throw() {
      return y<x;
    }
    friend bool operator!=(
      XxPair const& x, 
      XxPair const& y) throw() {
      return (x<y)||(y<x);
    }
    friend bool operator==(
      XxPair const& x, 
      XxPair const& y) throw() {
      return !(x!=y);
    }
    friend bool operator<=(
      XxPair const& x, 
      XxPair const& y) throw() {
      return (x<y)||(x==y);
    }
    friend bool operator>=(
      XxPair const& x, 
      XxPair const& y) throw() {
      return (x>y)||(x==y);
    }
  };
  
  virtual ::p4::F::XxPair f2(
    ::p4::F::XxPair const& z) throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
};

}

#endif
