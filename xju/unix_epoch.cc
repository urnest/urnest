#include <xju/unix_epoch.hh>
#line 1 "/home/xju/urnest/xju/unix_epoch.hcp"
#line 11
#include <ctime> //impl

namespace xju
{
#line 17
std::chrono::system_clock::time_point unix_epoch() throw()
{
  // we just assume that time_t uses the unix epoch - test-unix_epoch.cc
  // verifies this
  return std::chrono::system_clock::from_time_t(0);
}

}
