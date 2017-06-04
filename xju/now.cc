#include <xju/now.hh>
#line 1 "/home/xju/urnest/xju/now.hcp"
namespace xju
{
#line 14
std::chrono::system_clock::time_point now() throw()
{
  return std::chrono::system_clock::now();
}

}
