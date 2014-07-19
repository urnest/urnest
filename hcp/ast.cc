//     -*- mode: c++ ; c-file-style: "xju" ; -*-
//
// Copyright (c) 2008
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
static const char rcsid[] = "$RCSfile: twt_cxx.el,v $ $Revision: 1.2 $";
#include <hcp/ast.hh>

#include <sstream>
#include <iterator>
#include <algorithm>
#include "xju/assert.hh"

namespace hcp_ast
{

std::string reconstruct(IRs const& x) throw() {
  std::ostringstream s;
  for(IRs::const_iterator i=x.begin(); i != x.end(); ++i) {
    s << reconstruct(**i);
  }
  return s.str();
}

std::string ClassDef::getClassName(std::vector<IR> const& items) throw()
{
  std::vector<IR>::const_iterator i(
    std::find_if(items.begin(), items.end(),
                 isA_<ClassName>));
  xju::assert_not_equal(i, items.end());
  std::string result;
  std::remove_copy_if((**i).begin(), (**i).end(),
                      std::back_inserter(result),
                      ::isspace);
  return result;
}

bool ClassDef::getIsTemplateSpeicialisation(std::vector<IR> const& items) throw()
{
  std::vector<IR>::const_iterator i(
    std::find_if(items.begin(), items.end(),
                 isA_<TemplateEmptyPreamble>));
  return i!=items.end();
}

std::string NamespaceDef::getNamespaceName(
  std::vector<IR> const& items) throw()
{
  std::vector<IR>::const_iterator i(
    std::find_if(items.begin(), items.end(),
                 isA_<NamespaceName>));
  xju::assert_not_equal(i, items.end());
  std::string result;
  std::remove_copy_if((**i).begin(), (**i).end(),
                      std::back_inserter(result),
                      ::isspace);
  return result;
}


}
