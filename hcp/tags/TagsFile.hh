#ifndef _TAGSFILE_HH
#define _TAGSFILE_HH
// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <hcp/tags/Lookup.hh>
#include <hcp/tags/Namespace.hh>
#include "xju/path.hh"
#include "xju/Exception.hh"
namespace hcp
{
namespace tags
{

class TagsFile : public Lookup
{
public:
  xju::path::AbsolutePath const d_;
  xju::path::FileName const f_;

  explicit TagsFile(xju::path::AbsolutePath const& d,
                    xju::path::FileName const& f) throw();


  void reload() throw();

  
  Lookup::Locations lookupSymbol(NamespaceNames const& fromScope,
                                 NamespaceNames const& symbolScope,
                                 UnqualifiedSymbol const& symbol) throw();

  
private:
  xju::Mutex guard_;
  Namespace root_;
};
  
}
}
#endif
