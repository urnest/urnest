// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/ssh/transport/chooseAlgorithm.hh>

#include <iostream>
#include <xju/assert.hh>
#include <xju/ssh/transport/CipherName.hh>

namespace xju
{
namespace ssh
{
namespace transport
{

void test1(){
  CipherName C1("C1");
  CipherName C2("C2");
  CipherName C3("C3");
  CipherName C4("C4");

  {
    auto const& x(chooseAlgorithm<CipherName>(
                    "cipher",
                    {C1,C2},
                    {C2,C1}));
    xju::assert_equal(x,C1);
  }
  {
    auto const& x(chooseAlgorithm<CipherName>(
                    "cipher",
                    {C1,C2},
                    {C2,C3}));
    xju::assert_equal(x,C2);
  }
  try
  {
    auto const& x(chooseAlgorithm<CipherName>(
                    "cipher",
                     {C1,C2},
                     {C4,C3}));
    xju::assert_never_reached();
  }
  catch(xju::Exception const& e){
    xju::assert_equal(readableRepr(e),"Failed to choose cipher given client requested C1; C2 and server supports C3; C4 because\nclient and server have no cipher in common.");
  }
}



}
}
}

using namespace xju::ssh::transport;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

