// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/io/OBuf.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/pipe.hh>
#include <vector>
#include <xju/steadyNow.hh>
#include <cinttypes>

namespace xju
{
namespace io
{
using namespace std::chrono_literals;

void test1() {
  auto const deadline(xju::steadyNow()+10s);
  auto p(xju::pipe(true,true));

  OBuf x(*p.second,deadline,6U);
  
  std::vector<uint8_t> const data{1,2,3,4,5,6,'+','-'};

  auto b(x.flush(0));
  xju::assert_equal(b.second-b.first,6U);
  std::copy(data.begin(),data.begin()+6,b.first);
  b=x.flush(b.second);
  {
    std::vector<uint8_t> rdata(8U);
    rdata.resize(p.first->read(&rdata[0],rdata.size(),xju::steadyNow()));
    xju::assert_equal(rdata.size(),6U);
    xju::assert_equal(std::vector<uint8_t>(data.begin(),data.begin()+6),
                      rdata);
  }
  xju::assert_equal(b.second-b.first,6U);
  std::copy(data.begin()+6,data.begin()+7,b.first);
  b=x.flush(b.first+1);
  {
    std::vector<uint8_t> rdata(8U);
    rdata.resize(p.first->read(&rdata[0],rdata.size(),xju::steadyNow()));
    xju::assert_equal(rdata.size(),1U);
    xju::assert_equal(std::vector<uint8_t>(data.begin()+6,data.begin()+7),
                      rdata);
  }
}

void test2(){
  auto p(xju::pipe(true,true));
  std::vector<uint8_t> fill(1024U);
  while(p.second->write(&fill[0],fill.size(),xju::steadyNow())==fill.size());
  
  auto const deadline(xju::steadyNow()+10ms);
  OBuf x(*p.second,deadline,6U);
  auto b(x.flush(0));
  
  try{
    auto c=x.flush(b.first+1);
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"");
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

