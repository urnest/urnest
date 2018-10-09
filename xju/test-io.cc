// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/io.hh>
#include <xju/pipe.hh>

#include <iostream>
#include <xju/assert.hh>
#include "xju/Thread.hh"

namespace xju
{
namespace io
{

size_t fillPipe(
  xju::io::OStream& pipeInput) throw()
{
  size_t pipeMax(0);
  size_t thisWrite(0);
  do {
    thisWrite=pipeInput.write(std::vector<uint8_t>(1024,0).data(),
                              1024,
                              std::chrono::steady_clock::now());
    pipeMax+=thisWrite;
  }
  while(thisWrite==1024);
  return pipeMax;
}

// read/write
void test1() {
  auto p(xju::pipe(true,true));
  size_t const pipeMax(fillPipe(*p.second));

  // write timeout
  {
    auto t1(std::chrono::steady_clock::now());
    xju::assert_equal(
      p.second->write(std::vector<uint8_t>(1024,0).data(),
                      1024,
                      t1+std::chrono::milliseconds(100)),
      0);
    auto t2(std::chrono::steady_clock::now());
    xju::assert_greater_equal(t2-t1,std::chrono::milliseconds(100));
    xju::assert_less(t2-t1,std::chrono::milliseconds(200));
  }
  // read data already available with 0 timeout
  {
    std::vector<uint8_t> r(pipeMax+1,0);
    size_t read(p.first->read(
                  r.data(),pipeMax+1,std::chrono::steady_clock::now()));
    xju::assert_equal(read,pipeMax);
  }
  // read timeout, nothing read
  {
    std::vector<uint8_t> r(1,0);
    auto t1(std::chrono::steady_clock::now());
    xju::assert_equal(
      p.first->read(r.data(),r.size(),t1+std::chrono::milliseconds(100)),
      0);
    auto t2(std::chrono::steady_clock::now());
    xju::assert_greater_equal(t2-t1,std::chrono::milliseconds(100));
    xju::assert_less(t2-t1,std::chrono::milliseconds(200));
  }
  // data becomes available before timeout
  {
    std::vector<uint8_t> r(512,0);
    auto t1(std::chrono::steady_clock::now());
    xju::Thread th([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        xju::assert_equal(
          p.second->write(r.data(),
                          r.size(),
                          t1+std::chrono::milliseconds(0)),
          r.size());
      });
    xju::assert_equal(
      p.first->read(r.data(),r.size(),t1+std::chrono::milliseconds(100)),
      r.size());
    auto t2(std::chrono::steady_clock::now());
    xju::assert_greater_equal(t2-t1,std::chrono::milliseconds(50));
    xju::assert_less(t2-t1,std::chrono::milliseconds(100));
  }
  // close other end before read timeout
  {
    std::vector<uint8_t> r(512,0);
    auto t1(std::chrono::steady_clock::now());
    xju::Thread th([&]() {
        xju::assert_equal(
          p.second->write(r.data(),
                          r.size(),
                          t1+std::chrono::milliseconds(0)),
          r.size());
        p.second.reset();
      });
    xju::assert_equal(
      p.first->read(r.data(),r.size(),t1+std::chrono::milliseconds(100)),
      r.size());
    auto t2(std::chrono::steady_clock::now());
    xju::assert_less(t2-t1,std::chrono::milliseconds(100));
    try {
      size_t const x(
        p.first->read(r.data(),r.size(),t1+std::chrono::milliseconds(100)));
      xju::assert_never_reached();
    }
    catch(xju::io::Input::Closed& e) {
      xju::assert_contains(readableRepr(e),p.first->str()+" closed");
      auto t2(std::chrono::steady_clock::now());
      xju::assert_less(t2-t1,std::chrono::milliseconds(100));
    }
  }
}

// select
void test2() noexcept
{
  auto p1(xju::pipe(true,true));
  auto p2(xju::pipe(true,true));
  
  xju::assert_equal(
    xju::io::select({p1.first.get(),p2.first.get()},
                    {p1.second.get(),p2.second.get()},
                    std::chrono::steady_clock::now()),
    std::pair<std::set<Input const*>,std::set<Output const*> >(
      {},
      {p1.second.get(),p2.second.get()}));

  size_t const pipeMax(fillPipe(*p1.second));

  xju::assert_equal(
    xju::io::select({p1.first.get(),p2.first.get()},
                    {p1.second.get(),p2.second.get()},
                    std::chrono::steady_clock::now()),
    std::pair<std::set<Input const*>,std::set<Output const*> >(
      {p1.first.get()},
      {p2.second.get()}));

  fillPipe(*p2.second);

  xju::assert_equal(
    xju::io::select({p1.first.get(),p2.first.get()},
                    {p1.second.get(),p2.second.get()},
                    std::chrono::steady_clock::now()),
    std::pair<std::set<Input const*>,std::set<Output const*> >(
      {p1.first.get(),p2.first.get()},
      {}));

  std::vector<uint8_t> r(pipeMax,0);
  xju::assert_equal(p2.first->read(r.data(),pipeMax/2,
                                   std::chrono::steady_clock::now()),
                    pipeMax/2);
  
  xju::assert_equal(
    xju::io::select({p1.first.get(),p2.first.get()},
                    {p1.second.get(),p2.second.get()},
                    std::chrono::steady_clock::now()),
    std::pair<std::set<Input const*>,std::set<Output const*> >(
      {p1.first.get(),p2.first.get()},
      {p2.second.get()}));

  xju::assert_equal(p2.first->read(r.data(),pipeMax/2,
                                   std::chrono::steady_clock::now()),
                    pipeMax/2);

  // return before timeout on readable/writable
  {
    auto t1(std::chrono::steady_clock::now());
    xju::assert_equal(
      xju::io::select({p1.first.get(),p2.first.get()},
                      {p1.second.get(),p2.second.get()},
                      t1+std::chrono::milliseconds(100)),
      std::pair<std::set<Input const*>,std::set<Output const*> >(
        {p1.first.get()},
        {p2.second.get()}));
    xju::assert_less_equal(
      std::chrono::steady_clock::now(),
      t1+std::chrono::milliseconds(100));
  }
  
  // inputs only
  xju::assert_equal(
    xju::io::select(std::set<Input const*>({p1.first.get(),p2.first.get()}),
                    std::chrono::steady_clock::now()),
    std::set<Input const*>(
      {p1.first.get()}));

  // outputs only
  xju::assert_equal(
    xju::io::select(std::set<Output const*>({p1.second.get(),p2.second.get()}),
                    std::chrono::steady_clock::now()),
    std::set<Output const*>(
      {p2.second.get()}));

  // timeout
  {
    auto t1(std::chrono::steady_clock::now());
    xju::assert_equal(
      xju::io::select({p2.first.get()},
                      {p1.second.get()},
                      t1+std::chrono::milliseconds(50)),
      std::pair<std::set<Input const*>,std::set<Output const*> >(
        {},
        {}));
    xju::assert_greater_equal(
      std::chrono::steady_clock::now(),
      t1+std::chrono::milliseconds(50));
  }

  // timeout
  {
    auto t1(std::chrono::steady_clock::now());
    xju::assert_equal(
      xju::io::select(std::set<Input const*>({p2.first.get()}),
                      t1+std::chrono::milliseconds(50)),
      std::set<Input const*>({}));
    xju::assert_greater_equal(
      std::chrono::steady_clock::now(),
      t1+std::chrono::milliseconds(50));
  }

  // timeout
  {
    auto t1(std::chrono::steady_clock::now());
    xju::assert_equal(
      xju::io::select(std::set<Output const*>({p1.second.get()}),
                      t1+std::chrono::milliseconds(50)),
      std::set<Output const*>({}));
    xju::assert_greater_equal(
      std::chrono::steady_clock::now(),
      t1+std::chrono::milliseconds(50));
  }


}

}
}

using namespace xju::io;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

