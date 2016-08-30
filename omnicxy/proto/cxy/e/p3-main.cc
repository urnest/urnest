// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//


#include "p3.hh"
#include "p3.cref.hh"
#include "p3.sref.hh"

#include "E.hh"
#include <iostream>
#include <string>
#include <xju/format.hh>
#include <xju/stringToInt.hh>
#include <stdlib.h>
#include <xju/mt.hh>
#include <xju/Time.hh>
#include <cxy/ORB.hh>
#include "xju/Shared.hh"

std::string makeURI(int port, std::string const& objectName) throw()
{
  std::ostringstream s;
  s << "corbaloc:iiop:localhost:"<< port << "/" << objectName;
  return s.str();
}

class F_impl : public p3::F
{
public:
  ~F_impl() throw()
  {
  }
  
  virtual ::p3::MyInt f1(
    ::p3::MyInt const& a) throw(E)
  {
    std::cout << "::p3::f1(" 
              << a << ") -> MyInt(33)" << std::endl;
    calls_.push_back(xju::Shared<Call>(
                       new Call::f1(a)));
    return ::p3::MyInt(33);
  }
  virtual ::p3::MyString f2(::p3::MyString const& a) throw(E)
  {
    std::cout << "::p3::f2("
              << a << ") -> returning p3::f2" << std::endl;
    calls_.push_back(xju::Shared<Call>(
                       new Call::f2(a)));
    return ::p3::MyString("returning p3::f2");
  }
  virtual ::p3::MyFloat f3(::p3::MyFloat const& a) throw(E)
  {
    std::cout << "::p3::f3("
              << a << ") -> returning " << a << std::endl;
    calls_.push_back(xju::Shared<Call>(
                       new Call::f3(a)));
    return a;
  }
  virtual ::p3::MyDouble f4(::p3::MyDouble const& a) throw(E)
  {
    std::cout << "::p3::f4("
              << a << ") -> returning " << a << std::endl;
    calls_.push_back(xju::Shared<Call>(
                       new Call::f4(a)));
    return a;
  }
  struct Call
  {
    virtual ~Call() throw()
    {
    }
    struct f1;
    struct f2;
    struct f3;
    struct f4;
  };
  struct Call::f1 : Call
  {
    ~f1() throw()
    {
    }
    f1(::p3::MyInt const& a) throw():
        a_(a) {
    }
    ::p3::MyInt a_;
    
    friend bool operator==(f1 const& x, f1 const& y) throw()
    {
      return x.a_ == y.a_;
    }
  };
  struct Call::f2 : Call
  {
    ~f2() throw()
    {
    }
    f2(::p3::MyString const& a) throw():
        a_(a) {
    }
    ::p3::MyString a_;
    
    friend bool operator==(f2 const& x, f2 const& y) throw()
    {
      return x.a_ == y.a_;
    }
  };
  struct Call::f3 : Call
  {
    ~f3() throw()
    {
    }
    f3(::p3::MyFloat const& a) throw():
        a_(a) {
    }
    ::p3::MyFloat a_;
    
    friend bool operator==(f3 const& x, f3 const& y) throw()
    {
      return x.a_ == y.a_;
    }
  };
  struct Call::f4 : Call
  {
    ~f4() throw()
    {
    }
    f4(::p3::MyDouble const& a) throw():
        a_(a) {
    }
    ::p3::MyDouble a_;
    
    friend bool operator==(f4 const& x, f4 const& y) throw()
    {
      return x.a_ == y.a_;
    }
  };
  
  std::vector<xju::Shared<Call> > calls_;
};

  
int main(int argc, char* argv[])
{
  try {
    if (argc != 3 || !(std::string("client")==argv[2]||
                       std::string("server")==argv[2]||
                       std::string("both")==argv[2])) {
      std::cerr << "usage:  " 
                << argv[0] << " <ip-port> [client|server|both]" << std::endl;
      return 1;
    }
    
    std::string const OBJECT_NAME("p3");
    
    int const port(xju::stringToInt(argv[1]));
    
    if (argv[2]==std::string("client")) {
      cxy::ORB<E> orb("giop:tcp::");
      cxy::cref<p3::F> ref(orb, makeURI(port, OBJECT_NAME));
      std::cout << ref->f1(::p3::MyInt(8)) << std::endl;
      std::cout << ref->f2(::p3::MyString("fred")) << std::endl;
      std::cout << ref->f3(::p3::MyFloat(3.75)) << std::endl;
      std::cout << ref->f4(::p3::MyDouble(3.75)) << std::endl;
    }
    else if (argv[2]==std::string("server")) {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<E> orb(orbEndPoint);

      F_impl x;
      
      cxy::sref<p3::F> const xa(orb, OBJECT_NAME, x);
      
      orb.monitorUntil(xju::Time::now()+xju::MicroSeconds(30*1000000));
    }
    else
    {
      std::string const orbEndPoint="giop:tcp::"+xju::format::str(port);
      cxy::ORB<E> orb(orbEndPoint);
      F_impl x;
      
      cxy::sref<p3::F> const xa(orb, OBJECT_NAME, x);
      
      cxy::cref<p3::F> ref(orb, makeURI(port, OBJECT_NAME));
      xju::assert_equal(ref->f1(::p3::MyInt(1)),::p3::MyInt(33));
      xju::assert_equal(ref->f2(::p3::MyString("fred")),
                        ::p3::MyString("returning p3::f2"));
      xju::assert_equal(ref->f3(::p3::MyFloat(3.75)),
                        ::p3::MyFloat(3.75));
      xju::assert_equal(ref->f4(::p3::MyDouble(8.75)),
                        ::p3::MyDouble(8.75));
      xju::assert_equal(x.calls_.size(),4U);
      {
        F_impl::Call::f1 const& c(
          dynamic_cast<F_impl::Call::f1 const&>(*x.calls_[0]));
        xju::assert_equal(c, F_impl::Call::f1(::p3::MyInt(1)));
      }
      {
        F_impl::Call::f2 const& c(
          dynamic_cast<F_impl::Call::f2 const&>(*x.calls_[1]));
        xju::assert_equal(c, F_impl::Call::f2(::p3::MyString("fred")));
      }
      {
        F_impl::Call::f3 const& c(
          dynamic_cast<F_impl::Call::f3 const&>(*x.calls_[2]));
        xju::assert_equal(c, F_impl::Call::f3(::p3::MyFloat(3.75)));
      }
      {
        F_impl::Call::f4 const& c(
          dynamic_cast<F_impl::Call::f4 const&>(*x.calls_[3]));
        xju::assert_equal(c, F_impl::Call::f4(::p3::MyDouble(8.75)));
      }

      try {
        ::p3::MyInt x(std::numeric_limits<uint32_t>::max());
        ++x;
        xju::assert_never_reached();
      }
      catch(E const& e) {
        std::ostringstream s;
        s << "++" << x << " would overflow";
        xju::assert_equal(readableRepr(e), s.str());
      }

      try {
        ::p3::MyFloat x(std::numeric_limits<float>::max());
        x=x+::p3::MyFloat(1.0);
        xju::assert_never_reached();
      }
      catch(E const& e) {
        std::ostringstream s;
        s << x << " + 1 would overflow";
        xju::assert_equal(readableRepr(e), s.str());
      }
    }
    return 0;
  }
  catch(E& e) {
    e.addContext(xju::format::join(argv, argv+argc, " "), 
                 std::make_pair(__FILE__, __LINE__));
    std::cerr << readableRepr(e, true, false) << std::endl;
    return 1;
  }
}
