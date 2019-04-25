// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/linux/disk-space/trackLevel.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace linux
{
namespace disk_space
{

void test1() {
  FileSystemStat counts{1000,//free
                        1000,//total
                        1};
  std::set<Percent> const percentLevels{Percent(0),Percent(50),Percent(75)};
  xju::assert_equal(trackLevel(percentLevels.find(Percent(0)),
                               percentLevels,
                               counts),
                    percentLevels.find(Percent(0)));

  counts.blocksFree_=501;
  xju::assert_equal(trackLevel(percentLevels.find(Percent(0)),
                               percentLevels,
                               counts),
                    percentLevels.find(Percent(0)));
  @@@;
}

}
}
}

using namespace xju::linux::disk_space;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

