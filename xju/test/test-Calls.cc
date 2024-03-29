// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/test/Calls.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/Exception.hh>

namespace xju
{
namespace test
{

void test0() {
  Calls c;

  class A
  {
  public:
    void f1() {}
    int f2() {return 0;}
  };
  
  A a;
  {
    c.enqueue(callTo(a,&A::f1)());
    auto call{c.awaitCall(a,&A::f1,xju::steadyNow())};
    call->return_();
    call->awaitReturn(xju::steadyNow());
  }
  {
    c.enqueue(callTo(a,&A::f1)());
    auto call{c.awaitCall(a,&A::f1,xju::steadyNow())};
    xju::Exception e{"fred",XJU_TRACED};
    call->raise(e);
    try{
      call->awaitReturn(xju::steadyNow());
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e2){
      xju::assert_equal(e2,e);
    }
  }
  {
    c.enqueue(callTo(a,&A::f2)());
    auto call{c.awaitCall(a,&A::f2,xju::steadyNow())};
    call->return_(3);
    xju::assert_equal(call->awaitResult(xju::steadyNow()),3);
  }
  {
    c.enqueue(callTo(a,&A::f2)());
    auto call{c.awaitCall(a,&A::f2,xju::steadyNow())};
    xju::Exception e{"fred",XJU_TRACED};
    call->raise(e);
    try{
      call->awaitResult(xju::steadyNow());
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e2){
      xju::assert_equal(e2,e);
    }
  }
}

void test1() {
  Calls c;

  class A
  {
  public:
    void f1(int x) {}
    int f2(int x) {return x;}
    void f3(std::string const& x) {};
    void f4(std::string& x) {};
  };
  
  A a;
  {
    c.enqueue(callTo(a,&A::f1)(3));
    auto call{c.awaitCall(a,&A::f1,xju::steadyNow())};
    xju::assert_equal(call->p1_,3);
    call->return_();
    call->awaitReturn(xju::steadyNow());
  }
  {
    c.enqueue(callTo(a,&A::f1)(4));
    auto call{c.awaitCall(a,&A::f1,xju::steadyNow())};
    xju::Exception e{"fred",XJU_TRACED};
    call->raise(e);
    try{
      call->awaitReturn(xju::steadyNow());
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e2){
      xju::assert_equal(e2,e);
    }
  }
  {
    c.enqueue(callTo(a,&A::f2)(5));
    auto call{c.awaitCall(a,&A::f2,xju::steadyNow())};
    xju::assert_equal(call->p1_,5);
    call->return_(3);
    xju::assert_equal(call->awaitResult(xju::steadyNow()),3);
  }
  {
    c.enqueue(callTo(a,&A::f2)(5));
    auto call{c.awaitCall(a,&A::f2,xju::steadyNow())};
    xju::Exception e{"fred",XJU_TRACED};
    call->raise(e);
    try{
      call->awaitResult(xju::steadyNow());
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e2){
      xju::assert_equal(e2,e);
    }
  }
  {
    c.enqueue(callTo(a,&A::f3)(std::string("fred")));
    auto call{c.awaitCall(a,&A::f3,xju::steadyNow())};
    xju::assert_equal(call->p1_,std::string("fred"));
    call->return_();
    call->awaitReturn(xju::steadyNow());
  }
  {
    std::string fred("fred");
    c.enqueue(callTo(a,&A::f4)(fred));
    auto call{c.awaitCall(a,&A::f4,xju::steadyNow())};
    xju::assert_equal(&call->p1_,&fred);
    call->return_();
    call->awaitReturn(xju::steadyNow());
  }
}

void test2() {
  Calls c;

  class A
  {
  public:
    void f1(int x,char y) {}
    int f2(int x,char y) {return x;}
  };
  
  A a;
  {
    c.enqueue(callTo(a,&A::f1)(3,'a'));
    auto call{c.awaitCall(a,&A::f1,xju::steadyNow())};
    xju::assert_equal(call->p1_,3);
    xju::assert_equal(call->p2_,'a');
    call->return_();
    call->awaitReturn(xju::steadyNow());
  }
  {
    c.enqueue(callTo(a,&A::f1)(4,'b'));
    auto call{c.awaitCall(a,&A::f1,xju::steadyNow())};
    xju::Exception e{"fred",XJU_TRACED};
    call->raise(e);
    try{
      call->awaitReturn(xju::steadyNow());
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e2){
      xju::assert_equal(e2,e);
    }
  }
  {
    c.enqueue(callTo(a,&A::f2)(5,'b'));
    auto call{c.awaitCall(a,&A::f2,xju::steadyNow())};
    xju::assert_equal(call->p1_,5);
    xju::assert_equal(call->p2_,'b');
    call->return_(3);
    xju::assert_equal(call->awaitResult(xju::steadyNow()),3);
  }
  {
    c.enqueue(callTo(a,&A::f2)(5,'b'));
    auto call{c.awaitCall(a,&A::f2,xju::steadyNow())};
    xju::Exception e{"fred",XJU_TRACED};
    call->raise(e);
    try{
      call->awaitResult(xju::steadyNow());
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e2){
      xju::assert_equal(e2,e);
    }
  }
}

void test0c() {
  Calls c;

  class A
  {
  public:
    void f1() const {}
    int f2() const {return 0;}
  };
  
  A a;
  {
    c.enqueue(callTo(a,&A::f1)());
    auto call{c.awaitCall(a,&A::f1,xju::steadyNow())};
    call->return_();
    call->awaitReturn(xju::steadyNow());
  }
  {
    c.enqueue(callTo(a,&A::f1)());
    auto call{c.awaitCall(a,&A::f1,xju::steadyNow())};
    xju::Exception e{"fred",XJU_TRACED};
    call->raise(e);
    try{
      call->awaitReturn(xju::steadyNow());
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e2){
      xju::assert_equal(e2,e);
    }
  }
  {
    c.enqueue(callTo(a,&A::f2)());
    auto call{c.awaitCall(a,&A::f2,xju::steadyNow())};
    call->return_(3);
    xju::assert_equal(call->awaitResult(xju::steadyNow()),3);
  }
  {
    c.enqueue(callTo(a,&A::f2)());
    auto call{c.awaitCall(a,&A::f2,xju::steadyNow())};
    xju::Exception e{"fred",XJU_TRACED};
    call->raise(e);
    try{
      call->awaitResult(xju::steadyNow());
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e2){
      xju::assert_equal(e2,e);
    }
  }
}


void test1c() {
  Calls c;

  class A
  {
  public:
    void f1(int x) const {}
    int f2(int x) const {return x;}
    int f3(std::string& x) { return 1;};
    void f4(int& x) const {}
  };
  
  A a;
  {
    c.enqueue(callTo(a,&A::f1)(3));
    auto call{c.awaitCall(a,&A::f1,xju::steadyNow())};
    xju::assert_equal(call->p1_,3);
    call->return_();
    call->awaitReturn(xju::steadyNow());
  }
  {
    int p(7);
    c.enqueue(callTo(a,&A::f4)(p));
    auto call{c.awaitCall(a,&A::f4,xju::steadyNow())};
    xju::assert_equal(&call->p1_,&p);
    call->return_();
    call->awaitReturn(xju::steadyNow());
  }
  {
    c.enqueue(callTo(a,&A::f1)(4));
    auto call{c.awaitCall(a,&A::f1,xju::steadyNow())};
    xju::Exception e{"fred",XJU_TRACED};
    call->raise(e);
    try{
      call->awaitReturn(xju::steadyNow());
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e2){
      xju::assert_equal(e2,e);
    }
  }
  {
    c.enqueue(callTo(a,&A::f2)(5));
    auto call{c.awaitCall(a,&A::f2,xju::steadyNow())};
    xju::assert_equal(call->p1_,5);
    call->return_(3);
    xju::assert_equal(call->awaitResult(xju::steadyNow()),3);
  }
  {
    std::string jock("jock");
    c.enqueue(callTo(a,&A::f3)(jock));
    auto call{c.awaitCall(a,&A::f3,xju::steadyNow())};
    xju::assert_equal(&call->p1_,&jock);
    call->return_(3);
    xju::assert_equal(call->awaitResult(xju::steadyNow()),3);
  }
  {
    c.enqueue(callTo(a,&A::f2)(5));
    auto call{c.awaitCall(a,&A::f2,xju::steadyNow())};
    xju::Exception e{"fred",XJU_TRACED};
    call->raise(e);
    try{
      call->awaitResult(xju::steadyNow());
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e2){
      xju::assert_equal(e2,e);
    }
  }
}

void test2c() {
  Calls c;

  class A
  {
  public:
    void f1(int x,char y) const {}
    int f2(int x,char y) const {return x;}
  };
  
  A a;
  {
    c.enqueue(callTo(a,&A::f1)(3,'a'));
    auto call{c.awaitCall(a,&A::f1,xju::steadyNow())};
    xju::assert_equal(call->p1_,3);
    xju::assert_equal(call->p2_,'a');
    call->return_();
    call->awaitReturn(xju::steadyNow());
  }
  {
    c.enqueue(callTo(a,&A::f1)(4,'b'));
    auto call{c.awaitCall(a,&A::f1,xju::steadyNow())};
    xju::Exception e{"fred",XJU_TRACED};
    call->raise(e);
    try{
      call->awaitReturn(xju::steadyNow());
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e2){
      xju::assert_equal(e2,e);
    }
  }
  {
    c.enqueue(callTo(a,&A::f2)(5,'b'));
    auto call{c.awaitCall(a,&A::f2,xju::steadyNow())};
    xju::assert_equal(call->p1_,5);
    xju::assert_equal(call->p2_,'b');
    call->return_(3);
    xju::assert_equal(call->awaitResult(xju::steadyNow()),3);
  }
  {
    c.enqueue(callTo(a,&A::f2)(5,'b'));
    auto call{c.awaitCall(a,&A::f2,xju::steadyNow())};
    xju::Exception e{"fred",XJU_TRACED};
    call->raise(e);
    try{
      call->awaitResult(xju::steadyNow());
      xju::assert_never_reached();
    }
    catch(xju::Exception const& e2){
      xju::assert_equal(e2,e);
    }
  }
}

}
}

using namespace xju::test;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test0(), ++n;
  test1(), ++n;
  test2(), ++n;
  test0c(), ++n;
  test1c(), ++n;
  test2c(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

