#ifndef _OMNICXY_PROTO_P27_HH
#define _OMNICXY_PROTO_P27_HH
// generated from p27.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>


// included idl
#include <chrono>

namespace p27
{
class I 
{
public:
  virtual ~I() throw();
  struct R
  {
    
    ::std::chrono::nanoseconds ns_;
    ::std::chrono::microseconds us_;
    ::std::chrono::milliseconds ms_;
    ::std::chrono::seconds s_;
    ::std::chrono::minutes m_;
    ::std::chrono::hours h_;
  
    R(
      ::std::chrono::nanoseconds const& p1,
      ::std::chrono::microseconds const& p2,
      ::std::chrono::milliseconds const& p3,
      ::std::chrono::seconds const& p4,
      ::std::chrono::minutes const& p5,
      ::std::chrono::hours const& p6) throw();
    friend bool operator<(
      R const& x, 
      R const& y) throw() {
      if (x.ns_<y.ns_) return true;
      if (y.ns_<x.ns_) return false;
      if (x.us_<y.us_) return true;
      if (y.us_<x.us_) return false;
      if (x.ms_<y.ms_) return true;
      if (y.ms_<x.ms_) return false;
      if (x.s_<y.s_) return true;
      if (y.s_<x.s_) return false;
      if (x.m_<y.m_) return true;
      if (y.m_<x.m_) return false;
      if (x.h_<y.h_) return true;
      if (y.h_<x.h_) return false;
      return false;
    }
    friend bool operator>(
      R const& x, 
      R const& y) throw() {
      return y<x;
    }
    friend bool operator!=(
      R const& x, 
      R const& y) throw() {
      return (x<y)||(y<x);
    }
    friend bool operator==(
      R const& x, 
      R const& y) throw() {
      return !(x!=y);
    }
    friend bool operator<=(
      R const& x, 
      R const& y) throw() {
      return (x<y)||(x==y);
    }
    friend bool operator>=(
      R const& x, 
      R const& y) throw() {
      return (x>y)||(x==y);
    }
  };
  
  virtual ::p27::I::R f(
    ::std::chrono::nanoseconds const& ns_,
    ::std::chrono::microseconds const& us_,
    ::std::chrono::milliseconds const& ms_,
    ::std::chrono::seconds const& s_,
    ::std::chrono::minutes const& m_,
    ::std::chrono::hours const& h_) /*throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception)*/ = 0;
};

}

#endif
