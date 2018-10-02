// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ip/checksum.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace ip
{
namespace checksum
{
void test1() {
  std::vector<uint8_t> x{
    0x45, 0x00,
    0x00, 0x54, 0xbb, 0xdb, 0x40, 0x00, 0x40, 0x01,
    0x00, 0x00, 0xc0, 0xa8, 0x00, 0x03, 0xc0, 0xa8,
    0x00, 0x01};
  xju::assert_equal(calculate(
                      xju::ip::decode::makeIterator(x.begin(),x.end())),
                    0xfd78);
}
void test2() {
  std::vector<uint8_t> x{
    0x45, 0x00,
    0x00, 0x54, 0xbb, 0xdb, 0x40, 0x00, 0x40, 0x01,
    0xfd, 0x78, 0xc0, 0xa8, 0x00, 0x03, 0xc0, 0xa8,
    0x00, 0x01};
  validate(xju::ip::decode::makeIterator(x.begin(),x.end()));
}
void test3() {
  std::vector<uint8_t> x{
    0x45, 0x00,
    0x00, 0x54, 0xbb, 0xdb, 0x40, 0x00, 0x40, 0x01,
    0xfd, 0x79, 0xc0, 0xa8, 0x00, 0x03, 0xc0, 0xa8,
    0x00, 0x01};
  try{
    validate(xju::ip::decode::makeIterator(x.begin(),x.end()));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"invalid ip checksum.");
  }
}

}
}
}

using namespace xju::ip::checksum;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  test2(), ++n;
  test3(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

