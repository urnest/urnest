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
#include <utility>
namespace hcp
{
namespace tags
{

// Represents a namespace, which might contain symbols and child namespaces.
// not thread safe
class Namespace
{
public:
  // record locations of namespace_::symbol
  // - appends to any existing locations
  void addSymbol(std::vector<NamespaceName> const& namespace_,
                 UnqualifiedSymbol const& symbol,
                 std::vector<Location> const& locations) throw();


  class UnknownNamespace : public xju::Exception
  {
  public:
    //REVISIT: using xju::Exception;
    UnknownNamespace(const std::string& cause,
                     const xju::Traced& trace) throw();

  };
  class UnknownSymbol : public xju::Exception
  {
  public:
    //REVISIT: using xju::Exception;
    UnknownSymbol(const std::string& cause,
                  const xju::Traced& trace) throw();

  };

  // Lookup locations of namespace_::symbol when it is referenced
  // from scope fromScope_.
  //
  // eg to lookup X in
  //    namespace a { b::c::X f(); }
  // lookup( {'a'}, {'b','c'}, X) assuming *this is the root namespace
  std::vector<Location> lookup(std::vector<NamespaceName> const& fromScope,
                               std::vector<NamespaceName> const& namespace_,
                               UnqualifiedSymbol const& symbol) const throw(
                                 UnknownNamespace,
                                 UnknownSymbol);

      
        
private:
  typedef std::vector<Location> Locations;
  
  std::map<NamespaceName, Namespace> children_;
  std::map<UnqualifiedSymbol, Locations> symbols_;

  // implementation of lookup above
  std::vector<Location> lookup_(
    std::pair<std::vector<NamespaceName>::const_iterator,
              std::vector<NamespaceName>::const_iterator> const& fromScope,
                               std::vector<NamespaceName> const& namespace_,
                               UnqualifiedSymbol const& symbol) const throw(
                                 UnknownNamespace,
                                 UnknownSymbol);

  
    
  // find namespace given by path within this namespace, adding children where
  // necessary
  // post: path.size() || result===root
  Namespace& findNamespace(
    std::pair<std::vector<NamespaceName>::const_iterator,
              std::vector<NamespaceName>::const_iterator> const& path) throw();


  // find namespace given by path within *this
  // post: path.size() || result===*this
  Namespace const& findNamespace(
    std::pair<std::vector<NamespaceName>::const_iterator,
              std::vector<NamespaceName>::const_iterator> const& path) const
    throw(UnknownNamespace);

  
  std::vector<Location> const& findSymbol(
    UnqualifiedSymbol const& symbol) const throw(
      UnknownSymbol);


};

}
}
#endif
