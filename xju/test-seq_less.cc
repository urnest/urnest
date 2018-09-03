// Copyright (c) 2018 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/seq_less.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{

void test1() {
  std::vector<int> const x{1,2,3};
  {
    std::vector<int> const y{1,2,3};
    xju::assert_equal(xju::seq_less(x.begin(),x.end(),
                                    y.begin(),y.end()),false);
  }
  {
    std::vector<int> const y{1,3,3};
    xju::assert_equal(xju::seq_less(x.begin(),x.end(),
                                    y.begin(),y.end()),true);
  }
  {
    std::vector<int> const y{1,1,3};
    xju::assert_equal(xju::seq_less(x.begin(),x.end(),
                                    y.begin(),y.end()),false);
  }
  {
    std::vector<int> const y{1,2};
    xju::assert_equal(xju::seq_less(x.begin(),x.end(),
                                    y.begin(),y.end()),false);
  }
  {
    std::vector<int> const y{1,2,3,4};
    xju::assert_equal(xju::seq_less(x.begin(),x.end(),
                                    y.begin(),y.end()),true);
  }
}

}

using namespace xju;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

