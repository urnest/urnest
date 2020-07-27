#ifndef _OMNICXY_PROTO_P11_HH
#define _OMNICXY_PROTO_P11_HH
// generated from p11.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <memory>
#include <stdint.h>
#include <string>
#include <iostream>


extern std::string const p11_Z ;
extern int16_t const p11_Q ;
class X
{
public:
  enum Value {
    A,
    B,
    C
  };
  X(Value v) /*throw(cxy::Exception)*/;
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
  // throws std::bad_alloc and exceptions of case-type copy constructor
  virtual std::unique_ptr< U1 > clone() const=0;
protected:
  explicit U1(::X d) throw();
private:
  ::X d_;
  friend ::X discriminator(U1 const& x) throw() {
    return x.d_;
  }
  virtual bool lessThan(U1 const& b) const throw()=0;

  friend bool operator<(U1 const& a, U1 const& b) throw()
  {
    return a.lessThan(b);
  }
  friend bool operator>(U1 const& a, U1 const& b) throw()
  {
    return b < a;
  }
  friend bool operator!=(U1 const& a, U1 const& b) throw()
  {
    return (a<b)||(b<a);
  }
  friend bool operator==(U1 const& a, U1 const& b) throw()
  {
    return !(a<b)&&!(b<a);
  }
  friend bool operator<=(U1 const& a, U1 const& b) throw()
  {
    return (a<b)||(a==b);
  }
  friend bool operator>=(U1 const& a, U1 const& b) throw()
  {
    return (a>b)||(a==b);
  }
};

namespace p11
{
class F 
{
public:
  virtual ~F() throw();
  virtual ::std::shared_ptr< ::U1 const> f1(
    ::std::shared_ptr< ::U1 const> const& a) /*throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception)*/ = 0;
  class Y
  {
  public:
    enum Value {
      U,
      V
    };
    Y(Value v) /*throw(cxy::Exception)*/;
    friend Value valueOf(Y const& x) throw(){ return x.v_; }
  private:
    Value v_;
    friend bool operator<(Y const& x, Y const& y) throw(){
      return x.v_ < y.v_;
    }
    friend bool operator>(Y const& x, Y const& y) throw(){
      return x.v_ > y.v_;
    }
    friend bool operator==(Y const& x, Y const& y) throw(){
      return x.v_ == y.v_;
    }
    friend bool operator!=(Y const& x, Y const& y) throw(){
      return x.v_ != y.v_;
    }
    friend bool operator<=(Y const& x, Y const& y) throw(){
      return x.v_ <= y.v_;
    }
    friend bool operator>=(Y const& x, Y const& y) throw(){
      return x.v_ >= y.v_;
    }
    friend std::ostream& operator<<(std::ostream& s, Y const& x) throw(){
      switch(x.v_){
      case U: return s << "U";
      case V: return s << "V";
      }
      return s << (x.v_);
    }
  };
  
   // IDL Union U2
  class U2
  {
  public:
    virtual ~U2() throw();
    class U;
    class V;
    // throws std::bad_alloc and exceptions of case-type copy constructor
    virtual std::unique_ptr< U2 > clone() const=0;
  protected:
    explicit U2(::p11::F::Y d) throw();
  private:
    ::p11::F::Y d_;
    friend ::p11::F::Y discriminator(U2 const& x) throw() {
      return x.d_;
    }
    virtual bool lessThan(U2 const& b) const throw()=0;
  
    friend bool operator<(U2 const& a, U2 const& b) throw()
    {
      return a.lessThan(b);
    }
    friend bool operator>(U2 const& a, U2 const& b) throw()
    {
      return b < a;
    }
    friend bool operator!=(U2 const& a, U2 const& b) throw()
    {
      return (a<b)||(b<a);
    }
    friend bool operator==(U2 const& a, U2 const& b) throw()
    {
      return !(a<b)&&!(b<a);
    }
    friend bool operator<=(U2 const& a, U2 const& b) throw()
    {
      return (a<b)||(a==b);
    }
    friend bool operator>=(U2 const& a, U2 const& b) throw()
    {
      return (a>b)||(a==b);
    }
  };
  
};


class F::U2::U : 
  public U2
{
public:
  std::string u_;

  virtual ~U() throw();
  explicit U(
    std::string const& p1) throw();
  std::unique_ptr< F::U2 > clone() const;
  friend bool operator<(
    U const& x, 
    U const& y) throw() {
    if (x.u_<y.u_) return true;
    if (y.u_<x.u_) return false;
    return false;
  }
  friend bool operator>(
    U const& x, 
    U const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    U const& x, 
    U const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    U const& x, 
    U const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    U const& x, 
    U const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    U const& x, 
    U const& y) throw() {
    return (x>y)||(x==y);
  }
  bool lessThan(U2 const& b) const throw();
};

class F::U2::V : 
  public U2
{
public:
  int16_t v_;

  virtual ~V() throw();
  explicit V(
    int16_t const& p1) throw();
  std::unique_ptr< F::U2 > clone() const;
  friend bool operator<(
    V const& x, 
    V const& y) throw() {
    if (x.v_<y.v_) return true;
    if (y.v_<x.v_) return false;
    return false;
  }
  friend bool operator>(
    V const& x, 
    V const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    V const& x, 
    V const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    V const& x, 
    V const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    V const& x, 
    V const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    V const& x, 
    V const& y) throw() {
    return (x>y)||(x==y);
  }
  bool lessThan(U2 const& b) const throw();
};
}




class U1::A : 
  public U1
{
public:
  int32_t a_;

  virtual ~A() throw();
  explicit A(
    int32_t const& p1) throw();
  std::unique_ptr< U1 > clone() const;
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
  bool lessThan(U1 const& b) const throw();
};

class U1::B : 
  public U1
{
public:

  virtual ~B() throw();
  explicit B() throw();
  std::unique_ptr< U1 > clone() const;
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
  bool lessThan(U1 const& b) const throw();
};

class U1::C : 
  public U1
{
public:
  float c_;

  virtual ~C() throw();
  explicit C(
    float const& p1) throw();
  std::unique_ptr< U1 > clone() const;
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
  bool lessThan(U1 const& b) const throw();
};


#endif
