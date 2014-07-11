// generated from p1.idl by omni cxy idl backend specifying 
// xju::Exception from "xju/Exception.hh" as base class for all ipc exceptions

#ifndef P1_HH
#define P1_HH

#include "xju/Exception.hh"

namespace p1
{
class F
{
  virtual ~F() throw()
  {
  }
  virtual void f1() throw(
    // ipc failure
    xju::Exception) = 0;
};

}

#endif
