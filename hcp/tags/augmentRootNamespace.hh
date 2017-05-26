#ifndef _HCP_TAGS_AUGMENTROOTNAMESPACE_HCP
#define _HCP_TAGS_AUGMENTROOTNAMESPACE_HCP
// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <hcp/tags/Namespace.hh>

#include <utility>

#include <string>
#include <xju/path.hh>
namespace hcp
{
namespace tags
{

void augmentRootNamespace(Namespace& rootNamespace,
                          std::pair<hcp::tags::AbsolutePath, hcp::tags::FileName> const& tagsFileName,
                          bool const traceParser)
  throw(
    // pre: rootNamespace == rootNamespace@pre
    xju::Exception);


}
}
#endif
