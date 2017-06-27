#include "Address.hh"
#include <iostream> //impl

namespace xju
{
namespace ip
{
namespace v4
{
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
