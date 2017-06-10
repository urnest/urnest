#ifndef _P10_HH
#define _P10_HH
// generated from p10.idl by omni cxy idl backend specifying 
// cxy::Exception from <cxy/Exception.hh> as base class for all ipc exceptions

#include <cxy/Exception.hh>
#include <iostream>
namespace p10
{
class EN
{
public:
  enum Value {
    AA,
    BB,
    CC
  };
  EN(Value v) throw(cxy::Exception);

  friend Value valueOf(EN const& x) throw(){ return x.v_; }
private:
  Value v_;
  friend bool operator<(EN const& x, EN const& y) throw(){
    return x.v_ < y.v_;
  }
  friend bool operator>(EN const& x, EN const& y) throw(){
    return x.v_ > y.v_;
  }
  friend bool operator==(EN const& x, EN const& y) throw(){
    return x.v_ == y.v_;
  }
  friend bool operator!=(EN const& x, EN const& y) throw(){
    return x.v_ != y.v_;
  }
  friend bool operator<=(EN const& x, EN const& y) throw(){
    return x.v_ <= y.v_;
  }
  friend bool operator>=(EN const& x, EN const& y) throw(){
    return x.v_ >= y.v_;
  }
  friend std::ostream& operator<<(std::ostream& s, EN const& x) throw(){
    switch(x.v_){
    case AA: return s << "AA";
    case BB: return s << "BB";
    case CC: return s << "CC";
    }
    return s << (x.v_);
  }
};

class F 
{
public:
  virtual ~F() throw();

  virtual void f1(
    ::p10::EN const& a) throw(
    // ipc failure
    // - note servant may not throw
    cxy::Exception) = 0;
};

}
#endif
