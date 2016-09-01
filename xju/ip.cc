#include <xju/ip.hh>
#line 1 "/home/xju/urnest/xju/ip.hcp"
#line 16
#include <iostream> //impl

namespace xju
{
namespace ip
{
namespace v4
{
#line 34
std::ostream& operator<<(std::ostream& s, Address const& a) throw()
{
  return s << (int)((a.value()>>24) & 0xff) << "." 
           << (int)((a.value()>>16) & 0xff) << "." 
           << (int)((a.value()>> 8) & 0xff) << "." 
           << (int)((a.value()>> 0) & 0xff);
}

}
}
}
