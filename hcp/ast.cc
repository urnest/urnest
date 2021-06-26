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
#include <hcp/ast.hh>

#include <sstream>
#include <iterator>
#include <algorithm>
#include "xju/assert.hh"
#include <xju/format.hh>

namespace hcp_ast
{

std::string reconstruct(IRs const& x) throw() {
  std::ostringstream s;
  for(IRs::const_iterator i=x.begin(); i != x.end(); ++i) {
    s << reconstruct(**i);
  }
  return s.str();
}

std::string Item::str() const throw()
{
  if (!items_.size()){
    return xju::format::quote(std::string(begin().x_, end().x_));
  }
  std::vector<std::string> ss;
  for(std::vector<IR>::const_iterator i=items_.begin();
      i!=items_.end();
      ++i) {
    ss.push_back("\n  "+xju::format::indent((*i)->str(),"  "));
  }
  return "[ "+xju::format::join(ss.begin(),ss.end(),",")+"]";
}

std::vector<Item const*> getContextAt(
  I i, Item const& within) throw()
{
  std::vector<Item const*> result;
  for(IRs::const_iterator j = within.items().begin();
      j != within.items().end();
      ++j) {
    if (((*j)->begin().x_ <= i.x_) && (i.x_ < (*j)->end().x_) &&
        (*j)->items().size()) {
      result.push_back(&(**j));
      std::vector<Item const*> children(
        getContextAt(i, (**j)));
      std::copy(children.begin(), children.end(), std::back_inserter(result));
    }
  }
  return result;
}

namespace
{
bool isNewline(char c) throw()
{
  switch(c) {
  case '\n':
  case '\r':
    return true;
  }
  return false;
}
}

std::string ClassDef::getClassName(std::vector<IR> const& items) throw()
{
  std::vector<IR>::const_iterator i(
    std::find_if(items.begin(), items.end(),
                 isA_<ClassName>));
  xju::assert_not_equal(i, items.end());

  std::string result;
  // REVISIT: looks nice but gives X<::Y> which is invalid
  // std::remove_copy_if((**i).begin(), (**i).end(),
  //                     std::back_inserter(result),
  //                     ::isspace);
  std::remove_copy_if((**i).begin(), (**i).end(),
                      std::back_inserter(result),
                      isNewline);
  std::string::iterator e(result.end());
  while((e != result.begin()) && ::isspace(*(e-1))) {
    --e;
  }
  return std::string(result.begin(), e);
}

bool ClassDef::getIsTemplateSpecialisation(std::vector<IR> const& items) throw()
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
