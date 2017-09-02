#include <hcp/tags/Lookup.hh>
#include <cxy/copyContext.hh> //impl

namespace hcp
{
namespace tags
{
Lookup::~Lookup() throw()
  {
  }
  
Lookup::FoundIn::FoundIn(
      ::hcp::tags::Lookup::Locations const& p1,
      ::hcp::tags::Lookup::Headers const& p2) throw():
        locations_(p1),
        headers_(p2) {
    }
    
}
}
