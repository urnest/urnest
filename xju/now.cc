#include <xju/now.hh>
namespace xju
{
std::chrono::system_clock::time_point now() throw()
{
  return std::chrono::system_clock::now();
}

}
