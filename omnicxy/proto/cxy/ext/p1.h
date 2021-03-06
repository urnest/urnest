#ifndef _P1_H
#define _P1_H
// generated from p1.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>



namespace p1
{
class F 
{
public:
  virtual ~F() throw();
  virtual void f1() throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
};

}

#endif
