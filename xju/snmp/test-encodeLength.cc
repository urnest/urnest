// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/snmp/encodeLength.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace snmp
{

void test1() {
  std::vector<uint8_t> x(256,0);
  xju::assert_equal(encodeLength(x.begin(),77),x.begin()+1);
  xju::assert_equal(std::vector<uint8_t>(x.begin(),x.begin()+1),
                    std::vector<uint8_t>( {77} ));
  xju::assert_equal(encodeLength(x.begin(),359),x.begin()+3);
  xju::assert_equal(std::vector<uint8_t>(x.begin(),x.begin()+3),
                    std::vector<uint8_t>( {0x82,0x01,0x67} ));
      
}

}
}

using namespace xju::snmp;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

