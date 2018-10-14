// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ip/icmp/decodeDestinationUnreachable.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace ip
{
namespace icmp
{

void test1() {
  DestinationUnreachable const u{decodeDestinationUnreachable(
      xju::ip::v4::Address( (192U<<24)+(168U<<16)+(0U<<8)+2),
      Message::Code(0x06),
      xju::Array<uint8_t,4>{
        0x00,
        0x00,
        0x01,
        0x02},
      std::vector<uint8_t>{
          // ip header
          0x45,
          0x00,
          0x00, 0x54,
          0xbb, 0xdb,
          0x40, 0x00, 0x40,
          0x01,
          0xfd, 0x78,
          0xc0, 0xa8, 0x00, 0x01,
          0xc0, 0xa8, 0x00, 0x03,
          
          0x08, 0x00, 0x63, 0x17,
          0x0e, 0xaf, 0x00, 0x01
          })};
  xju::assert_equal(
    u,
    DestinationUnreachable(
      xju::ip::v4::Address( (192U<<24)+(168U<<16)+(0U<<8)+2),
      Message::Code(0x06),
      xju::ip::v4::Address( (192U<<24)+(168U<<16)+(0U<<8)+3),
      xju::ip::Protocol(0x01),
      xju::Array<uint8_t,8>{
        0x08, 0x00, 0x63, 0x17,
          0x0e, 0xaf, 0x00, 0x01
          }));
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

