#include <xju/unix_epoch.hh>
#include <ctime> //impl

namespace xju
{
std::chrono::system_clock::time_point unix_epoch() throw()
{
  // we just assume that time_t uses the unix epoch - test-unix_epoch.cc
  // verifies this
  return std::chrono::system_clock::from_time_t(0);
}

}
