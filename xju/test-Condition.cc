// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/Condition.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/Mutex.hh>
#include <xju/Lock.hh>
#include <xju/Thread.hh>
#include <chrono>

namespace xju
{

// condition variable - wait
void test1() {
  const auto begin(std::chrono::steady_clock::now());
  xju::Mutex guard;
  xju::Condition c(guard);
  xju::Lock l(guard);
  xju::Thread t(
    [&](){
      xju::Lock l(guard);
      std::this_thread::sleep_for(std::chrono::seconds(1));
      c.signal(l);
    });
  c.wait(l);
  const auto end(std::chrono::steady_clock::now());
  
  const std::chrono::microseconds diff(
    std::chrono::duration_cast<std::chrono::microseconds>(end - begin));
  
  xju::assert_(diff, std::greater_equal<std::chrono::microseconds>(),
               std::chrono::microseconds(1000000));
  xju::assert_(diff, std::less<std::chrono::microseconds>(),
               std::chrono::microseconds(1500000));
}


// condition variable - timed wait
void test2()
{
  xju::Mutex m;
  xju::Condition c(m);
  bool flag(false);
  
  const std::chrono::steady_clock::time_point n(std::chrono::steady_clock::now());
  
  xju::Lock l(m);
  
  xju::Thread thread(
    [&](){
      std::this_thread::sleep_for(std::chrono::seconds(5));
      xju::Lock l(m);
      flag=true;
      c.signal(l);
    });
  
  while(std::chrono::steady_clock::now() < n+std::chrono::seconds(2))
  {
    c.wait(l, n+std::chrono::seconds(2));
  }
  xju::assert_equal(flag, false);
  
  while(!flag && std::chrono::steady_clock::now() < n+std::chrono::seconds(6))
  {
    c.wait(l, n+std::chrono::seconds(5));
  }
  xju::assert_equal(flag, true);
  xju::assert_(
    std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - n), std::less<std::chrono::microseconds>(), std::chrono::seconds(6));
}

}

using namespace xju;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}
