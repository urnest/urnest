#ifndef _P19_HH
#define _P19_HH
// generated from p19.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <stdint.h>
#include <string>
#include <cxy/optional.hh>
#include <vector>


struct E : cxy::Exception
{
  
  std::vector< std::string > cause;
  cxy::optional< int32_t > optionalNumber;

  ~E() throw();

  E(
    std::vector< std::string > const& p1,
    cxy::optional< int32_t > const& p2,
    // cxy::Exception params
    std::string const& cause, 
    std::pair<std::string, unsigned int> const& fileAndLine) throw();

  friend bool operator<(
    E const& x, 
    E const& y) throw() {
    if (x.cause<y.cause) return true;
    if (y.cause<x.cause) return false;
    if (x.optionalNumber<y.optionalNumber) return true;
    if (y.optionalNumber<x.optionalNumber) return false;
    return (cxy::Exception const&)x < (cxy::Exception const&)y;
  }
  friend bool operator>(
    E const& x, 
    E const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    E const& x, 
    E const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    E const& x, 
    E const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    E const& x, 
    E const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    E const& x, 
    E const& y) throw() {
    return (x>y)||(x==y);
  }
};

struct F
{
  
  std::vector< std::string > m;
  cxy::optional< int32_t > optionalNumber;

  F(
    std::vector< std::string > const& p1,
    cxy::optional< int32_t > const& p2) throw();

  friend bool operator<(
    F const& x, 
    F const& y) throw() {
    if (x.m<y.m) return true;
    if (y.m<x.m) return false;
    if (x.optionalNumber<y.optionalNumber) return true;
    if (y.optionalNumber<x.optionalNumber) return false;
    return false;
  }
  friend bool operator>(
    F const& x, 
    F const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    F const& x, 
    F const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    F const& x, 
    F const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    F const& x, 
    F const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    F const& x, 
    F const& y) throw() {
    return (x>y)||(x==y);
  }
};

#endif
