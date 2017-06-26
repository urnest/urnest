#include "splitSymbol.hh"
#include <algorithm> //impl

namespace hcp
{
namespace tags
{
namespace
{
std::string const sep{"::"};
}

// split C++ symbol, possibly qualified, into namespace names and
// (leaf) symbol name
// eg a::b -> { {a},b }
//    a::b::c ->  { {a,b},c }
// ... note leading :: is ignored
// eg ::a::b ->  { {a},b }
//
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
