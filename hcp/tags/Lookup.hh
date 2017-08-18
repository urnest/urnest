#ifndef _HCP_TAGS_LOOKUP_HH
#define _HCP_TAGS_LOOKUP_HH
//generated from "/home/xju/tmp/oc52-2/xjutv/FILES/d/m/Lookup.hcp.1840389.hcp"
// generated from Lookup.idl by omni cxy idl backend specifying 
// xju::Exception from <xju/Exception.hh> as base class for all ipc exceptions

#include <xju/Exception.hh>
#include <vector>

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
  virtual ::hcp::tags::Lookup::Locations lookupSymbol(
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
