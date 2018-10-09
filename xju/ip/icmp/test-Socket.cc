// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ip/icmp/Socket.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/ip/icmp/Echo.hh>
#include <xju/steadyNow.hh>
#include <xju/io/select.hh>
#include <xju/ip/icmp/encodeEcho.hh>
#include <xju/ip/icmp/decodeEcho.hh>

namespace xju
{
namespace ip
{
namespace icmp
{

void test1() {
  Socket s;
  auto const pid{getpid()};
  Echo e{Echo::Identifier(pid),
         Echo::Sequence(1),
         {1,2,3}};
  Message m(Message::Type::ECHO,
            Message::Code(0),
            Checksum(0),
            encodeEcho(e));
  auto const deadline{xju::steadyNow()+std::chrono::seconds(1)};
  xju::ip::v4::Address localHost{127U<<24+0U<<16+0U<<8+1U};
  s.send(localHost,m,deadline);
  while(true){
    xju::assert_not_equal(
      xju::io::select({(xju::io::Input*)&s},deadline).size(),0U);
    auto const r{s.receive()};
    if (std::get<0>(r)==localHost &&
        std::get<1>(r).type_==Message::Type::ECHOREPLY &&
        std::get<1>(r).code_==Message::Code(0)){
      auto const er{decodeEcho(std::get<1>(r).header_,std::get<1>(r).data_)};
      if (er==e){
        return;
      }
    }
    xju::assert_less(xju::steadyNow(),deadline);
  }        
}

}
}
}

using namespace xju::ip::icmp;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

