#include "TagsFile.hh"
#include "xju/Lock.hh" //impl
#include "xju/Mutex.hh" //impl
#include <iostream> //impl
#include <sstream> //impl
#include <utility> //impl
#include <hcp/tags/augmentRootNamespace.hh> //impl

namespace hcp
{
namespace tags
{
TagsFile::TagsFile(xju::path::AbsolutePath const& d,
                    xju::path::FileName const& f) throw():
      d_(d),
      f_(f)
  {
    reload();
  }

  
void TagsFile::reload() throw()
  {
    xju::Lock l(guard_);
    try {
      root_=Namespace();
      augmentRootNamespace(root_,
                           std::make_pair(d_,f_),
                           false);
    }
    catch(xju::Exception& e) {
      std::ostringstream s;
      s << "reload tags file "
        << xju::path::str(d_,f_);
      e.addContext(s.str(),XJU_TRACED);
      std::cerr << "ERROR: " << readableRepr(e) << std::endl;
    }
  }
  
  
Lookup::Locations TagsFile::lookupSymbol(NamespaceNames const& fromScope,
                                 NamespaceNames const& symbolScope,
                                 UnqualifiedSymbol const& symbol) throw() {
    xju::Lock l(guard_);
    try {
      return root_.lookup(fromScope,symbolScope,symbol);
    }
    catch(xju::Exception& e) {
      std::ostringstream s;
      s << "lookup symbol in tags file "
        << xju::path::str(d_,f_);
      e.addContext(s.str(),XJU_TRACED);
      std::cerr << readableRepr(e) << std::endl;
    }
    return Locations();
  }
  

}
}
