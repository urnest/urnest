#include <hcp/tags/splitSymbol.hh>
#line 1 "/home/xju/urnest/hcp/tags/splitSymbol.hcp"
#line 15
#include <algorithm> //impl

namespace hcp
{
namespace tags
{
namespace
{
  std::string const sep("::");
}

std::pair<std::vector<NamespaceName>,UnqualifiedSymbol> splitSymbol(
  std::string const& symbol) throw()
{
  auto i(symbol.begin());
  if (symbol.size()>=2 && std::string(i,i+2)=="::") {
    i+=2;
  }
  auto j(std::search(i,symbol.end(),
                     sep.begin(),sep.end()));
  std::vector<NamespaceName> a;
  while(j!=symbol.end()) {
    a.push_back(NamespaceName(std::string(i,j)));
    i=j+2;
    j=std::search(i,symbol.end(),sep.begin(),sep.end());
  }
  return std::make_pair(a, UnqualifiedSymbol(std::string(i,j)));
}


}
}
