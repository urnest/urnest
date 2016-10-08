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

namespace xju
{
namespace io
{

void test1() {
  auto p(xju::pipe(true,true));
  size_t pipeMax(0);
  size_t thisWrite(0);
  do {
    thisWrite=p.second->write(std::vector<uint8_t>(1024,0).data(),
                              1024,
                              std::chrono::system_clock::now());
    pipeMax+=thisWrite;
  }
  while(thisWrite==1024);

  auto t1(std::chrono::system_clock::now());
  xju::assert_equal(
    p.second->write(std::vector<uint8_t>(1024,0).data(),
                    1024,
                    t1+std::chrono::milliseconds(100)),
    0);
  auto t2(std::chrono::system_clock::now());
  xju::assert_greater_equal(t2-t1,std::chrono::milliseconds(100));
  xju::assert_less(t2-t1,std::chrono::milliseconds(200));
  {
    std::vector<uint8_t> r(pipeMax+1,0);
    size_t read(p.first->read(
                  r.data(),pipeMax+1,std::chrono::system_clock::now()));
    xju::assert_equal(read,pipeMax);
    auto t1(std::chrono::system_clock::now());
    xju::assert_equal(
      p.first->read(r.data(),1,t1+std::chrono::milliseconds(100)),
      0);
    auto t2(std::chrono::system_clock::now());
    xju::assert_greater_equal(t2-t1,std::chrono::milliseconds(100));
    xju::assert_less(t2-t1,std::chrono::milliseconds(200));
  }
}

}
}

using namespace xju::io;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

