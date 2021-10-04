// Copyright (c) 2020 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <dion/job/Ids.hh>

#include <iostream>
#include <xju/assert.hh>

using namespace dion::job;

int main(int argc, char* argv[])
{
  Ids const x;

  Ids y(Id(1));
  xju::assert_equal(y.onlyJob(),Id(1));
  xju::assert_equal(y-x, y);
  xju::assert_equal(y+x, y);
  xju::assert_equal(y&x, x);

  Ids z;
  xju::assert_equal(z, x);
  z.add(Ids(Id(1)));
  xju::assert_equal(y, z);
  z.remove(Ids(Id(2)));
  xju::assert_equal(y, z);
  z.remove(Ids(Id(1)));
  xju::assert_equal(z, x);

  Ids q(Id(2));

  z.add(Id(1));
  z.add(Id(2));

  xju::assert_equal(y+q, z);
  
  xju::assert_less(x, Ids(Ids::maxId_));
  
  std::cout << "PASS" << std::endl;
  return 0;
}
