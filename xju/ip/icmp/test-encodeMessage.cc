// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ip/icmp/encodeMessage.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace ip
{
namespace icmp
{

void test1() {
  std::vector<uint8_t> data;
  auto i{encodeMessage(
      Message(
        Message::Type(8),
        Message::Code(0),
        Checksum(0x6317),
        xju::Array<uint8_t,4>{
          0x0e,
            0xaf,
            0x00,0x01},
        std::vector<uint8_t>{
          0x6f, 0x74, 0xaf, 0x5b, 0x00, 0x00,
            0x00, 0x00, 0xa6, 0x95, 0x02, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
            0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
            0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
            0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d,
            0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
            0x36, 0x37}),
      std::back_inserter(data))};
  xju::assert_equal(
    data,
    std::vector<uint8_t>{
      0x08, 0x00, 0x63, 0x17, 0x0e, 0xaf,
        0x00, 0x01, 0x6f, 0x74, 0xaf, 0x5b, 0x00, 0x00,
        0x00, 0x00, 0xa6, 0x95, 0x02, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
        0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
        0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
        0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d,
        0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
        0x36, 0x37});
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

