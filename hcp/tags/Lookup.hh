#ifndef _HCP_TAGS_LOOKUP_HH
#define _HCP_TAGS_LOOKUP_HH
// generated from Lookup.idl by omni cxy idl backend specifying 
// xju::Exception from <xju/Exception.hh> as base class for all ipc exceptions

#include <xju/Exception.hh>
#include <string>
#include <vector>
#include <xju/Tagged.hh>

// included idl
#include <hcp/tags/NamespaceName.hh>
#include <hcp/tags/AbsolutePath.hh>
#include <hcp/tags/Location.hh>
#include <hcp/tags/UnqualifiedSymbol.hh>
#include <hcp/tags/FileName.hh>
#include <hcp/tags/DirName.hh>
#include <hcp/tags/LineNumber.hh>

namespace hcp
{
namespace tags
{
class Lookup 
{
public:
  virtual ~Lookup() throw();
  typedef std::vector< ::hcp::tags::NamespaceName > NamespaceNames;
  typedef std::vector< ::hcp::tags::Location > Locations;
  class Header_tag {};
  typedef ::xju::Tagged<std::string,Header_tag > Header;
  typedef std::vector< ::hcp::tags::Lookup::Header > Headers;
  struct FoundIn
  {
    
    ::hcp::tags::Lookup::Locations locations_;
    ::hcp::tags::Lookup::Headers headers_;
  
    FoundIn(
      ::hcp::tags::Lookup::Locations const& p1,
      ::hcp::tags::Lookup::Headers const& p2) throw();
    friend bool operator<(
      FoundIn const& x, 
      FoundIn const& y) throw() {
      if (x.locations_<y.locations_) return true;
      if (y.locations_<x.locations_) return false;
      if (x.headers_<y.headers_) return true;
      if (y.headers_<x.headers_) return false;
      return false;
    }
    friend bool operator>(
      FoundIn const& x, 
      FoundIn const& y) throw() {
      return y<x;
    }
    friend bool operator!=(
      FoundIn const& x, 
      FoundIn const& y) throw() {
      return (x<y)||(y<x);
    }
    friend bool operator==(
      FoundIn const& x, 
      FoundIn const& y) throw() {
      return !(x!=y);
    }
    friend bool operator<=(
      FoundIn const& x, 
      FoundIn const& y) throw() {
      return (x<y)||(x==y);
    }
    friend bool operator>=(
      FoundIn const& x, 
      FoundIn const& y) throw() {
      return (x>y)||(x==y);
    }
  };
  
  virtual ::hcp::tags::Lookup::FoundIn lookupSymbol(
    ::hcp::tags::Lookup::NamespaceNames const& fromScope,
    ::hcp::tags::Lookup::NamespaceNames const& symbolScope,
    ::hcp::tags::UnqualifiedSymbol const& symbol) throw(
    // ipc failure
    // - note servant may not throw
    xju::Exception) = 0;
};

}
}

#endif
