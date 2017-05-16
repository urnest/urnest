#ifndef _HCP_TAGS_NAMESPACE_HCP
#define _HCP_TAGS_NAMESPACE_HCP
// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <vector>
#include <hcp/tags/NamespaceName.hh>
#include <hcp/tags/UnqualifiedSymbol.hh>
#include <hcp/tags/Location.hh>
#include "xju/Exception.hh"
#include <map>

namespace hcp
{
namespace tags
{

class Namespace
{
public:
  void addSymbol(std::vector<NamespaceName> const& namespace_,
                 UnqualifiedSymbol const& symbol,
                 std::vector<Location> const& locations) throw();


  class UnknownNamespace : public xju::Exception
  {
  public:
    
  };
  class UnknownSymbol : public xju::Exception
  {
  public:
  };
  
  
  std::vector<Location> lookup(std::vector<NamespaceName> const& namespace_,
                               UnqualifiedSymbol const& symbol) const throw(
                                 UnknownNamespace,
                                 UnknownSymbol);
    
private:
  typedef std::vector<Location> Locations;
  
  std::map<NamespaceName, Namespace> children_;
  std::map<UnqualifiedSymbol, Locations> symbols_;

  // find namespace given by path within root, adding children where
  // necessary
  // post: path.size() || result===root
  Namespace& findNamespace(Namespace& root,
                           std::vector<NamespaceName> const& path) throw();

  // find namespace given by path within root, adding children where
  // necessary
  // post: path.size() || result===root
  Namespace const& findNamespace(Namespace const& root,
                                 std::vector<NamespaceName> const& path) throw(
                                   UnknownNamespace);
  
  Locations const& findSymbol(
    std::map<UnqualifiedSymbol, Locations> const& symbols) throw(
      UnknownSymbol);
  
};

}
}
#endif
