// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/io/IBuf.hh>

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
#include <xju/net/istream.hh>

namespace xju
{
namespace io
{
using namespace std::chrono_literals;

void test1() {
  std::vector<uint8_t> x{1,2,3,4,5,6,'+','-'};
  auto p(xju::pipe(true,true));
  p.second->write(&x[0],x.size(),xju::steadyNow());
  xju::io::IBuf buf(*p.first,xju::steadyEternity(),6);
  
  auto y(buf.underflow());
  xju::assert_equal(std::vector<uint8_t>(y.first,y.second),
                    std::vector<uint8_t>(x.begin(),x.begin()+6));
  y=buf.underflow();
  xju::assert_equal(std::vector<uint8_t>(y.first,y.second),
                    std::vector<uint8_t>(x.begin()+6,x.begin()+8));

  // deadline reached
  {
    xju::io::IBuf::DeadlineOverride deadline2(buf,xju::steadyNow()+20ms);
    try{
      y=buf.underflow();
      xju::assert_never_reached();
    }
    catch(xju::DeadlineReached const& e){
    }
  }
  xju::assert_equal(p.second->write("10.",3,xju::steadyNow()),3U);
  y=buf.underflow();
  xju::assert_equal(std::string(y.first,y.second),"10.");
  
  p.second.reset();
  try{
    y=buf.underflow();
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

