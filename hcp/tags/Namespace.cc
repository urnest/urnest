#include "Namespace.hh"
#include <algorithm> //impl
#include "xju/next.hh" //impl
#include <sstream> //impl
#include <xju/format.hh> //impl

namespace hcp
{
namespace tags
{
void Namespace::addSymbol(std::vector<NamespaceName> const& namespace_,
                 UnqualifiedSymbol const& symbol,
                 std::vector<Location> const& locations) throw() {
    Namespace& n(findNamespace( {namespace_.begin(),namespace_.end()} ));
    auto i=n.symbols_.find(symbol);
    if (i==n.symbols_.end()) {
      i=n.symbols_.insert( {symbol,Locations() }).first;
    }
    std::copy(locations.begin(),locations.end(),
              std::back_inserter((*i).second));
  }

  
Namespace::UnknownNamespace::UnknownNamespace(const std::string& cause,
                     const xju::Traced& trace) throw():
        xju::Exception(cause,trace)
    {
    }
  
Namespace::UnknownSymbol::UnknownSymbol(const std::string& cause,
                  const xju::Traced& trace) throw():
        xju::Exception(cause,trace)
    {
    }
  
std::vector<Location> Namespace::lookup(std::vector<NamespaceName> const& fromScope,
                               std::vector<NamespaceName> const& namespace_,
                               UnqualifiedSymbol const& symbol) const throw(
                                 UnknownNamespace,
                                 UnknownSymbol)
  {
    try {
      return lookup_( std::make_pair(fromScope.begin(),fromScope.end()),
                      namespace_,
                      symbol);
    }
    catch(xju::Exception& e) {
      std::ostringstream s;
      s << "lookup locations of "
        << xju::format::join(namespace_.begin(),namespace_.end(),"::")
        << "::" << symbol
        << " when it is referenced from "
        << xju::format::join(fromScope.begin(),fromScope.end(),"::") << "::";
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
  }
      
        

std::vector<Location> Namespace::lookup_(
    std::pair<std::vector<NamespaceName>::const_iterator,
              std::vector<NamespaceName>::const_iterator> const& fromScope,
                               std::vector<NamespaceName> const& namespace_,
                               UnqualifiedSymbol const& symbol) const throw(
                                 UnknownNamespace,
                                 UnknownSymbol)
  {
    if (fromScope.first!=fromScope.second) {
      auto const i=children_.find(*fromScope.first);
      if (i != children_.end()) {
        try {
          return (*i).second.lookup_(
            std::make_pair(xju::next(fromScope.first),fromScope.second),
            namespace_,
            symbol);
        }
        catch(UnknownSymbol const&) {
        }
        catch(UnknownNamespace const&) {
        }
      }
    }
    return findNamespace(
      std::make_pair(namespace_.begin(),namespace_.end())).findSymbol(
        symbol);
  }
  
    
  // find namespace given by path within this namespace, adding children where
  // necessary
  // post: path.size() || result===root
  
Namespace& Namespace::findNamespace(
    std::pair<std::vector<NamespaceName>::const_iterator,
              std::vector<NamespaceName>::const_iterator> const& path) throw()
  {
    if (path.first==path.second) {
      return *this;
    }
    auto i=children_.find(*path.first);
    if (i==children_.end()) {
      i=children_.insert( {*path.first,Namespace()} ).first;
    }
    return (*i).second.findNamespace( {xju::next(path.first),path.second} );
  }

  // find namespace given by path within *this
  // post: path.size() || result===*this
  
Namespace const& Namespace::findNamespace(
    std::pair<std::vector<NamespaceName>::const_iterator,
              std::vector<NamespaceName>::const_iterator> const& path) const
    throw(UnknownNamespace)
  {
    try {
      if (path.first==path.second) {
        return *this;
      }
      auto i=children_.find(*path.first);
      if (i==children_.end()) {
        std::vector<NamespaceName> known;
        for(auto x:children_) {
          known.push_back(x.first);
        }
        std::ostringstream s;
        s << "unknown namespace " << (*path.first)
          << ", not one of "
          << xju::format::join(known.begin(),known.end(),",");
        throw UnknownNamespace(s.str(),XJU_TRACED);
      }
      return (*i).second.findNamespace({xju::next(path.first),path.second});
    }
    catch(xju::Exception& e) {
      std::ostringstream s;
      s << "find namespace " << xju::format::join(path.first,path.second,"::");
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
  }
  
  
std::vector<Location> const& Namespace::findSymbol(
    UnqualifiedSymbol const& symbol) const throw(
      UnknownSymbol)
  {
    try {
      auto i=symbols_.find(symbol);
      if (i==symbols_.end()) {
        throw UnknownSymbol("unknown symbol",XJU_TRACED);
      }
      return (*i).second;
    }
    catch(xju::Exception& e) {
      std::vector<UnqualifiedSymbol> known;
      for(auto x:symbols_) {
        known.push_back(x.first);
      }
      std::ostringstream s;
      s << "find symbol " << symbol << " amongst "
        << xju::format::join(known.begin(),known.end(),",");
      e.addContext(s.str(),XJU_TRACED);
      throw;
    }
  }


}
}
