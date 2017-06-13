#include "str.hh"
#include <sstream> //impl
namespace hcp
{
namespace tags
{
std::string str(std::vector<NamespaceName> const& x) throw()
{
  std::ostringstream s;
  for(auto const n: x) {
    s << n._ << "::";
  }
  return s.str();
}
std::string str(UnqualifiedSymbol const& x) throw()
{
  return x._;
}


}
}
