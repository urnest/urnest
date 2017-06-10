#ifndef _P11_HH
#define _P11_HH
// generated from p11.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <iostream>
#include <string>
#include <stdint.h>
#include <xju/Shared.hh>
extern 
std::string const p11_Z ;
extern int16_t const p11_Q ;
class X
{
public:
  enum Value {
    A,
    B,
    C
  };
  X(Value v) throw(cxy::Exception);

  friend Value valueOf(X const& x) throw(){ return x.v_; }
private:
  Value v_;
  friend bool operator<(X const& x, X const& y) throw(){
    return x.v_ < y.v_;
  }
  friend bool operator>(X const& x, X const& y) throw(){
    return x.v_ > y.v_;
  }
  friend bool operator==(X const& x, X const& y) throw(){
    return x.v_ == y.v_;
  }
  friend bool operator!=(X const& x, X const& y) throw(){
    return x.v_ != y.v_;
  }
  friend bool operator<=(X const& x, X const& y) throw(){
    return x.v_ <= y.v_;
  }
  friend bool operator>=(X const& x, X const& y) throw(){
    return x.v_ >= y.v_;
  }
  friend std::ostream& operator<<(std::ostream& s, X const& x) throw(){
    switch(x.v_){
    case A: return s << "A";
    case B: return s << "B";
    case C: return s << "C";
    }
    return s << (x.v_);
  }
};

 // IDL Union U1
class U1
{
public:
  virtual ~U1() throw();

  class A;
  class B;
  class C;
protected:
  explicit U1(::X d) throw();

private:
  ::X d_;
  friend ::X discriminator(U1 const& x) throw() {
    return x.d_;
  }
};

class U1::A : 
  public U1
{
public:
  int32_t a_;

  virtual ~A() throw();

  explicit A(
    int32_t const& p1) throw();

  friend bool operator<(
    A const& x, 
    A const& y) throw() {
    if (x.a_<y.a_) return true;
    if (y.a_<x.a_) return false;
    return false;
  }
  friend bool operator>(
    A const& x, 
    A const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    A const& x, 
    A const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    A const& x, 
    A const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    A const& x, 
    A const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    A const& x, 
    A const& y) throw() {
    return (x>y)||(x==y);
  }
};

class U1::B : 
  public U1
{
public:

  virtual ~B() throw();

  explicit B() throw();

  friend bool operator<(
    B const& x, 
    B const& y) throw() {
    return false;
  }
  friend bool operator>(
    B const& x, 
    B const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    B const& x, 
    B const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    B const& x, 
    B const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    B const& x, 
    B const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    B const& x, 
    B const& y) throw() {
    return (x>y)||(x==y);
  }
};

class U1::C : 
  public U1
{
public:
  float c_;

  virtual ~C() throw();

  explicit C(
    float const& p1) throw();

  friend bool operator<(
    C const& x, 
    C const& y) throw() {
    if (x.c_<y.c_) return true;
    if (y.c_<x.c_) return false;
    return false;
  }
  friend bool operator>(
    C const& x, 
    C const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    C const& x, 
    C const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    C const& x, 
    C const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    C const& x, 
    C const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    C const& x, 
    C const& y) throw() {
    return (x>y)||(x==y);
  }
};


bool operator<(U1::A const& x, U1 const& b) throw();

bool operator<(U1::B const& x, U1 const& b) throw();

bool operator<(U1::C const& x, U1 const& b) throw();

bool operator<(U1 const& a, U1 const& b) throw();

bool operator>(U1 const& a, U1 const& b) throw();

bool operator!=(U1 const& a, U1 const& b) throw();

bool operator==(U1 const& a, U1 const& b) throw();

bool operator<=(U1 const& a, U1 const& b) throw();

bool operator>=(U1 const& a, U1 const& b) throw();


namespace p11
{
class F 
{
public:
  virtual ~F() throw();

  virtual void f1(
    ::xju::Shared< ::U1 const> const& a) throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
};

}
#endif
