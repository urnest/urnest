#ifndef _OMNICXY_PROTO_P23_HH
#define _OMNICXY_PROTO_P23_HH
// generated from p23.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <string>


class A 
{
public:
  virtual ~A() throw();
  virtual std::string fa() throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
};

class B :
  public virtual A
{
public:
  virtual ~B() throw();
  virtual std::string fb() throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
};

class C :
  public virtual A
{
public:
  virtual ~C() throw();
  virtual std::string fc() throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
};

class D :
  public virtual B,
  public virtual C
{
public:
  virtual ~D() throw();

};





#endif
