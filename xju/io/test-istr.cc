// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/io/istr.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/readThrough.hh>
#include <xju/format.hh>
#include <algorithm>
#include <sstream>
#include <xju/pipe.hh>
#include <xju/steadyEternity.hh>
#include <xju/Thread.hh>
#include <xju/steadyNow.hh>
#include <xju/DeadlineReached.hh>

namespace xju
{
namespace io
{
using namespace std::chrono_literals;

void test1() {
  // feed some data into pipe, read a bit at a time within deadline
  int i=0;
  std::ostringstream s;
  for(i=0;i!=10;++i){
    s << i << ".";
  }
  auto const ss(s.str());
  xju::ByteBuffer b(ss.size());
  std::copy(ss.begin(),ss.end(),b.contigSpace().first);
  b.appended(ss.size());
  auto p(xju::pipe(true,true));
  
  xju::io::istr is(*p.first,xju::steadyEternity(),std::move(b));
  for(i=0;i!=10;++i){
    auto const x(xju::readThrough(is,".",25));
    xju::assert_equal(x,xju::format::int_(i)+".");
  }

  // deadline reached
  {
    xju::io::istr::DeadlineOverride deadline2(is,xju::steadyNow()+20ms);
    try{
      auto const x(xju::readThrough(is,".",25));
      xju::assert_never_reached();
    }
    catch(xju::DeadlineReached const& e){
    }
  }
  is.clear();
  xju::assert_equal(p.second->write("10.",3,xju::steadyNow()),3U);
  for(i=10;i!=11;++i){
    auto const x(xju::readThrough(is,".",25));
    xju::assert_equal(x,xju::format::int_(i)+".");
  }
  {
    xju::Thread t(
      [&](){
        for(int i=11; i!=1000; ++i){
          auto const s(xju::format::str(i)+".");
          p.second->write(s.c_str(),s.size(),xju::steadyEternity());
        }
      });
    for(i=11;i!=1000;++i){
      auto const x(xju::readThrough(is,".",25));
      xju::assert_equal(x,xju::format::int_(i)+".");
    }
  }
  p.second.reset();
  try{
    auto const x(xju::readThrough(is,".",25));
    xju::assert_never_reached();
  }
  catch(xju::io::Input::Closed const& e){
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

