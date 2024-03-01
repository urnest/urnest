// Copyright (c) 2022 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/crypt/ciphers/DEScbcCipher.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace crypt
{
namespace ciphers
{

void test1() {
  DEScbcCipher c(xju::crypt::CipherKey({1,2,3,4,5,6,7,8}),
                    xju::crypt::CipherIV({2,3,4,5,6,7,8,9}));
  xju::assert_equal(c.decryptData(
                      c.encryptData({1,2,3,1,2,3,9,8,7,0,0,0,0,0,0,0})),
                    std::vector<uint8_t>({1,2,3,1,2,3,9,8,7,0,0,0,0,0,0,0}));
}

}
}
}

using namespace xju::crypt::ciphers;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

