#ifndef _XJU_LINUX_WTMP_EVENT_HH
#define _XJU_LINUX_WTMP_EVENT_HH
// generated from event.idl by omni cxy idl backend specifying 
// xju::Exception from <xju/Exception.hh> as base class for all ipc exceptions

#include <xju/Exception.hh>
#include <memory>
#include <string>
#include <iostream>
#include <cxy/UnixTimeMilli.hh>

namespace xju
{
namespace linux
{
namespace wtmp
{
namespace event
{
struct UserLoggedIn
{
  
  std::string username_;
  std::string from_;
  ::cxy::UnixTimeMilli at_;

  UserLoggedIn(
    std::string const& p1,
    std::string const& p2,
    ::cxy::UnixTimeMilli const& p3) throw();
  friend bool operator<(
    UserLoggedIn const& x, 
    UserLoggedIn const& y) throw() {
    if (x.username_<y.username_) return true;
    if (y.username_<x.username_) return false;
    if (x.from_<y.from_) return true;
    if (y.from_<x.from_) return false;
    if (x.at_<y.at_) return true;
    if (y.at_<x.at_) return false;
    return false;
  }
  friend bool operator>(
    UserLoggedIn const& x, 
    UserLoggedIn const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    UserLoggedIn const& x, 
    UserLoggedIn const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    UserLoggedIn const& x, 
    UserLoggedIn const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    UserLoggedIn const& x, 
    UserLoggedIn const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    UserLoggedIn const& x, 
    UserLoggedIn const& y) throw() {
    return (x>y)||(x==y);
  }
};

struct UserLoggedOut
{
  
  ::xju::linux::wtmp::event::UserLoggedIn who_;
  ::cxy::UnixTimeMilli at_;

  UserLoggedOut(
    ::xju::linux::wtmp::event::UserLoggedIn const& p1,
    ::cxy::UnixTimeMilli const& p2) throw();
  friend bool operator<(
    UserLoggedOut const& x, 
    UserLoggedOut const& y) throw() {
    if (x.who_<y.who_) return true;
    if (y.who_<x.who_) return false;
    if (x.at_<y.at_) return true;
    if (y.at_<x.at_) return false;
    return false;
  }
  friend bool operator>(
    UserLoggedOut const& x, 
    UserLoggedOut const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    UserLoggedOut const& x, 
    UserLoggedOut const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    UserLoggedOut const& x, 
    UserLoggedOut const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    UserLoggedOut const& x, 
    UserLoggedOut const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    UserLoggedOut const& x, 
    UserLoggedOut const& y) throw() {
    return (x>y)||(x==y);
  }
};

class Type
{
public:
  enum Value {
    USER_LOGGED_IN,
    USER_LOGGED_OUT
  };
  Type(Value v) throw(xju::Exception);
  friend Value valueOf(Type const& x) throw(){ return x.v_; }
private:
  Value v_;
  friend bool operator<(Type const& x, Type const& y) throw(){
    return x.v_ < y.v_;
  }
  friend bool operator>(Type const& x, Type const& y) throw(){
    return x.v_ > y.v_;
  }
  friend bool operator==(Type const& x, Type const& y) throw(){
    return x.v_ == y.v_;
  }
  friend bool operator!=(Type const& x, Type const& y) throw(){
    return x.v_ != y.v_;
  }
  friend bool operator<=(Type const& x, Type const& y) throw(){
    return x.v_ <= y.v_;
  }
  friend bool operator>=(Type const& x, Type const& y) throw(){
    return x.v_ >= y.v_;
  }
  friend std::ostream& operator<<(std::ostream& s, Type const& x) throw(){
    switch(x.v_){
    case USER_LOGGED_IN: return s << "USER_LOGGED_IN";
    case USER_LOGGED_OUT: return s << "USER_LOGGED_OUT";
    }
    return s << (x.v_);
  }
};

 // IDL Union Event_
class Event_
{
public:
  virtual ~Event_() throw();
  class USER_LOGGED_IN;
  class USER_LOGGED_OUT;
  // throws std::bad_alloc and exceptions of case-type copy constructor
  virtual std::unique_ptr< Event_ > clone() const=0;
protected:
  explicit Event_(::xju::linux::wtmp::event::Type d) throw();
private:
  ::xju::linux::wtmp::event::Type d_;
  friend ::xju::linux::wtmp::event::Type discriminator(Event_ const& x) throw() {
    return x.d_;
  }
  virtual bool lessThan(Event_ const& b) const throw()=0;

  friend bool operator<(Event_ const& a, Event_ const& b) throw()
  {
    return a.lessThan(b);
  }
  friend bool operator>(Event_ const& a, Event_ const& b) throw()
  {
    return b < a;
  }
  friend bool operator!=(Event_ const& a, Event_ const& b) throw()
  {
    return (a<b)||(b<a);
  }
  friend bool operator==(Event_ const& a, Event_ const& b) throw()
  {
    return !(a<b)&&!(b<a);
  }
  friend bool operator<=(Event_ const& a, Event_ const& b) throw()
  {
    return (a<b)||(a==b);
  }
  friend bool operator>=(Event_ const& a, Event_ const& b) throw()
  {
    return (a>b)||(a==b);
  }
};


class Event_::USER_LOGGED_IN : 
  public Event_
{
public:
  ::xju::linux::wtmp::event::UserLoggedIn userLoggedIn_;

  virtual ~USER_LOGGED_IN() throw();
  explicit USER_LOGGED_IN(
    ::xju::linux::wtmp::event::UserLoggedIn const& p1) throw();
  std::unique_ptr< Event_ > clone() const;
  friend bool operator<(
    USER_LOGGED_IN const& x, 
    USER_LOGGED_IN const& y) throw() {
    if (x.userLoggedIn_<y.userLoggedIn_) return true;
    if (y.userLoggedIn_<x.userLoggedIn_) return false;
    return false;
  }
  friend bool operator>(
    USER_LOGGED_IN const& x, 
    USER_LOGGED_IN const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    USER_LOGGED_IN const& x, 
    USER_LOGGED_IN const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    USER_LOGGED_IN const& x, 
    USER_LOGGED_IN const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    USER_LOGGED_IN const& x, 
    USER_LOGGED_IN const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    USER_LOGGED_IN const& x, 
    USER_LOGGED_IN const& y) throw() {
    return (x>y)||(x==y);
  }
  bool lessThan(Event_ const& b) const throw();
};

class Event_::USER_LOGGED_OUT : 
  public Event_
{
public:
  ::xju::linux::wtmp::event::UserLoggedOut userLoggedOut_;

  virtual ~USER_LOGGED_OUT() throw();
  explicit USER_LOGGED_OUT(
    ::xju::linux::wtmp::event::UserLoggedOut const& p1) throw();
  std::unique_ptr< Event_ > clone() const;
  friend bool operator<(
    USER_LOGGED_OUT const& x, 
    USER_LOGGED_OUT const& y) throw() {
    if (x.userLoggedOut_<y.userLoggedOut_) return true;
    if (y.userLoggedOut_<x.userLoggedOut_) return false;
    return false;
  }
  friend bool operator>(
    USER_LOGGED_OUT const& x, 
    USER_LOGGED_OUT const& y) throw() {
    return y<x;
  }
  friend bool operator!=(
    USER_LOGGED_OUT const& x, 
    USER_LOGGED_OUT const& y) throw() {
    return (x<y)||(y<x);
  }
  friend bool operator==(
    USER_LOGGED_OUT const& x, 
    USER_LOGGED_OUT const& y) throw() {
    return !(x!=y);
  }
  friend bool operator<=(
    USER_LOGGED_OUT const& x, 
    USER_LOGGED_OUT const& y) throw() {
    return (x<y)||(x==y);
  }
  friend bool operator>=(
    USER_LOGGED_OUT const& x, 
    USER_LOGGED_OUT const& y) throw() {
    return (x>y)||(x==y);
  }
  bool lessThan(Event_ const& b) const throw();
};
}
}
}
}

#endif
