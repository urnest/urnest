#ifndef P5_HCP
#define P5_HCP
// generated from p5.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <stdint.h>

// included idl
#include "p4.hh"
#include "p3.hh"

namespace p5
{
typedef std::vector< ::p4::XS1> JW2s;
typedef std::vector< uint32_t> LS;
class F
{
public:
  virtual ~F() throw()
  ;
virtual ::p5::JW2s f1(
    ::p5::JW2s const& p1) throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
};

}
#endif
