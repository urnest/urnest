// Copyright (c) 2019 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/linux/ntpd/parseNtpqAssociations.hh>

#include <iostream>
#include <xju/assert.hh>

namespace xju
{
namespace linux
{
namespace ntpd
{

void test1() {
  std::string const x{R"--(
ind assid status  conf reach auth condition  last_event cnt
===========================================================
  1 42977  8811   yes  none  none    reject    mobilize  1
  2 42978  8811   yes  none  none    reject    mobilize  1
  3 42979  8811   yes  none  none    reject    mobilize  1
  4 42980  8811   yes  none  none    reject    mobilize  1
  5 42981  151a    no   yes  none    backup    sys_peer  1
  6 42982  1414    no   yes  none candidate   reachable  1
  7 42983  142a    no   yes  none candidate    sys_peer  2
  8 42984  1514    no   yes  none    backup   reachable  1
  9 42985  1514    no   yes  none    backup   reachable  1
 10 42986  1414    no   yes  none candidate   reachable  1
 11 42987  1414    no   yes  none candidate   reachable  1
 12 42988  1414    no   yes  none candidate   reachable  1
 13 42989  1414    no   yes  none candidate   reachable  1
 14 42990  1414    no   yes  none candidate   reachable  1
 15 42991  1414    no   yes  none candidate   reachable  1
 16 42992  1414    no   yes  none candidate   reachable  1
 17 42993  1514    no   yes  none    backup   reachable  1
 18 42994  143a    no   yes  none candidate    sys_peer  3
 19 42995  161a    no   yes  none  sys.peer    sys_peer  1
)--"};
  xju::assert_equal(parseNtpqAssociations(x),
                    std::map<AssocId,PeerStatus>{
                      {AssocId(42977),PeerStatus(0x8811)},
                      {AssocId(42978),PeerStatus(0x8811)},
                      {AssocId(42979),PeerStatus(0x8811)},
                      {AssocId(42980),PeerStatus(0x8811)},
                      {AssocId(42981),PeerStatus(0x151a)},
                      {AssocId(42982),PeerStatus(0x1414)},
                      {AssocId(42983),PeerStatus(0x142a)},
                      {AssocId(42984),PeerStatus(0x1514)},
                      {AssocId(42985),PeerStatus(0x1514)},
                      {AssocId(42986),PeerStatus(0x1414)},
                      {AssocId(42987),PeerStatus(0x1414)},
                      {AssocId(42988),PeerStatus(0x1414)},
                      {AssocId(42989),PeerStatus(0x1414)},
                      {AssocId(42990),PeerStatus(0x1414)},
                      {AssocId(42991),PeerStatus(0x1414)},
                      {AssocId(42992),PeerStatus(0x1414)},
                      {AssocId(42993),PeerStatus(0x1514)},
                      {AssocId(42994),PeerStatus(0x143a)},
                      {AssocId(42995),PeerStatus(0x161a)}});
}

}
}
}

using namespace xju::linux::ntpd;

int main(int argc, char* argv[])
{
  unsigned int n(0);
  test1(), ++n;
  std::cout << "PASS - " << n << " steps" << std::endl;
  return 0;
}

