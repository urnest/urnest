// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ip/v4/encodeHeader.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace ip
{
namespace v4
{

void test1() {
  Header const h{
    Header::Version(4),
    Header::IHL(5),
    Header::DSCP(0),
    Header::ECN(0),
    Header::TotalLength(0x54),
    Header::Identification(0xbbdb),
    Header::Flags(2),
    Header::FragmentOffset(0),
    Header::TTL(0x40),
    Header::Protocol(1),
    Header::HeaderChecksum(0xfd78),
    Header::Address((192U<<24)+(168U<<16)+0U+3U),
    Header::Address((192U<<24)+(168U<<16)+0U+1U),
    {}};
  std::vector<uint8_t> x{
    0x45, 0x00,
    0x00, 0x54, 0xbb, 0xdb, 0x40, 0x00, 0x40, 0x01,
    0xfd, 0x78, 0xc0, 0xa8, 0x00, 0x03, 0xc0, 0xa8,
    0x00, 0x01};
  std::vector<uint8_t> y;
  encodeHeader(h,std::back_inserter(y));
  xju::assert_equal(y,x);
}

}
}
}

using namespace xju::ip::v4;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

