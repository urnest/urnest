#include <xju/linux/wtmp/event.hh>
#include <sstream> // impl

// included idl
#include <cxy/copyContext.hh> //impl

namespace xju
{
namespace linux
{
namespace wtmp
{
namespace event
{
UserLoggedIn::UserLoggedIn(
    std::string const& p1,
    std::string const& p2,
    ::cxy::UnixTimeMilli const& p3) throw():
      username_(p1),
      from_(p2),
      at_(p3) {
  }
  
UserLoggedOut::UserLoggedOut(
    ::xju::linux::wtmp::event::UserLoggedIn const& p1,
    ::cxy::UnixTimeMilli const& p2) throw():
      who_(p1),
      at_(p2) {
  }
  
Type::Type(Value v) throw(xju::Exception):
    v_(v) {
    if ((v < 0)||(v > 2)){
      std::ostringstream s;
      s << v << " is outside allowed range 0..2";
      throw xju::Exception(s.str(), std::make_pair(__FILE__,__LINE__));
    }
  }
  
Event_::~Event_() throw() {
  }
  
Event_::Event_(::xju::linux::wtmp::event::Type d) throw():
    d_(d){
  }

Event_::USER_LOGGED_IN::~USER_LOGGED_IN() throw() {
  }
  
Event_::USER_LOGGED_IN::USER_LOGGED_IN(
    ::xju::linux::wtmp::event::UserLoggedIn const& p1) throw():
      Event_(::xju::linux::wtmp::event::Type::USER_LOGGED_IN),
      userLoggedIn_(p1) {
  }
  
std::unique_ptr< Event_ > Event_::USER_LOGGED_IN::clone() const // override 
  {
    return std::unique_ptr< Event_ >(new USER_LOGGED_IN(*this));
  }
  
bool Event_::USER_LOGGED_IN::lessThan(Event_ const& b) const throw()
  {
    if (dynamic_cast<Event_::USER_LOGGED_IN const* >(&b)) {
      return (*this) < dynamic_cast< Event_::USER_LOGGED_IN const& >(b);
    }
    if (dynamic_cast<Event_::USER_LOGGED_OUT const* >(&b)) {
      return true;
    }
    return false;
  }

Event_::USER_LOGGED_OUT::~USER_LOGGED_OUT() throw() {
  }
  
Event_::USER_LOGGED_OUT::USER_LOGGED_OUT(
    ::xju::linux::wtmp::event::UserLoggedOut const& p1) throw():
      Event_(::xju::linux::wtmp::event::Type::USER_LOGGED_OUT),
      userLoggedOut_(p1) {
  }
  
std::unique_ptr< Event_ > Event_::USER_LOGGED_OUT::clone() const // override 
  {
    return std::unique_ptr< Event_ >(new USER_LOGGED_OUT(*this));
  }
  
bool Event_::USER_LOGGED_OUT::lessThan(Event_ const& b) const throw()
  {
    if (dynamic_cast<Event_::USER_LOGGED_OUT const* >(&b)) {
      return (*this) < dynamic_cast< Event_::USER_LOGGED_OUT const& >(b);
    }
    return false;
  }

}
}
}
}
