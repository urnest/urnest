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
#include <xju/path.hh>

#include <xju/next.hh>
#include <xju/format.hh>
#include <unistd.h>
#include "xju/prev.hh"
#include <algorithm>

namespace xju
{
namespace path
{

namespace 
{

std::vector<DirName> split_string(std::string const& x) throw()
{
  std::vector<DirName> result;
  std::string::const_iterator i = x.begin();
  while(i != x.end()) {
    std::string::const_iterator j = std::find(i, x.end(), '/');
    result.push_back(DirName(std::string(i, j)));
    i=j;
    while((i!=x.end() && ((*i) == '/'))) {
      ++i;
    }
  }
  return result;
}

std::vector<DirName> normalise(std::vector<DirName> const& x) /*throw(
  xju::Exception)*/
{
  try {
    std::vector<DirName> result;
    for(std::vector<DirName>::const_iterator i=x.begin();
        i != x.end();
        ++i) {
      if ((*i)._=="." || (*i)._=="") {
      }
      else if ((*i)._=="..") {
        if (!result.size()) {
          std::ostringstream s;
          s << "too many '..'s at component " << (i-x.begin()+1);
          throw xju::Exception(s.str(), XJU_TRACED);
        }
        result.erase(result.end()-1);
      }
      else {
        result.push_back(*i);
      }
    }
    return result;
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "normalise path " << xju::format::join(x.begin(), x.end(), "/");
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}
}

RelativePath::RelativePath(std::string x) /*throw(
  xju::Exception)*/ try:
  x_(split_string(x))
{
  if (x.size() && (x[0]=='/')) {
    std::ostringstream s;
    s << xju::format::quote(x) 
      << " starts with '/'";
    throw xju::Exception(s.str(), XJU_TRACED);
  }
}
catch(xju::Exception& e)
{
  std::ostringstream s;
  s << "validate relative path " << xju::format::quote(x);
  e.addContext(s.str(), XJU_TRACED);
  throw;
}

AbsolutePath::AbsolutePath(std::string const& x) /*throw(
  xju::Exception)*/ try
{
  if (!x.size() || (x[0]!='/')) {
    std::ostringstream s;
    s << xju::format::quote(x)
      << " does not start with '/'";
    throw xju::Exception(s.str(), XJU_TRACED);
  }
  x_=normalise(split_string(std::string(x.begin()+1, x.end())));
}
catch(xju::Exception& e)
{
  std::ostringstream s;
  s << "validate absolute path " << xju::format::quote(x);
  e.addContext(s.str(), XJU_TRACED);
  throw;
}

std::string str(RelativePath const& x) throw()
{
  std::vector<std::string> y;
  for(std::vector<DirName>::const_iterator i(x.x_.begin()); 
      i!=x.x_.end(); 
      ++i) {
    y.push_back((*i)._);
  }
  return xju::format::join(y.begin(), y.end(), "/");
}

AbsolutePath::AbsolutePath(std::vector<DirName> const& x) /*throw(
  xju::Exception)*/ try:
  x_(normalise(x))
{
}
catch(xju::Exception& e)
{
  std::ostringstream s;
  s << "normalise absolute path /" 
    << xju::format::join(x.begin(), x.end(), "/");
  e.addContext(s.str(), XJU_TRACED);
  throw;
}

std::ostream& operator<<(std::ostream& s,
                         AbsolutePath const& x) throw() {
  return s << str(x);
}

AbsolutePath operator+(AbsolutePath const& x, RelativePath const& y) /*throw(
  xju::Exception)*/
{
  std::vector<DirName> a;
  std::copy(x.begin(), x.end(), std::back_inserter(a));
  std::copy(y.begin(), y.end(), std::back_inserter(a));
  return AbsolutePath(a);
}

AbsolutePath operator+(AbsolutePath const& x, DirName const& y) /*throw(
  xju::Exception)*/
{
  std::vector<DirName> a;
  std::copy(x.begin(), x.end(), std::back_inserter(a));
  a.push_back(y);
  return AbsolutePath(a);
}

std::pair<AbsolutePath,FileName> operator+(
  AbsolutePath const& x,
  FileName const& y) /*throw(
    xju::Exception)*/
{
  return std::make_pair(x,y);
}

AbsolutePath root() throw()
{
  return AbsolutePath("/");
}
AbsolutePath working_dir() throw()
{
  char b[5000];
  return AbsolutePath(::getcwd(b, sizeof(b)));
}

std::string str(AbsolutePath const& x) throw()
{
  std::vector<std::string> y;
  for(std::vector<DirName>::const_iterator i(x.begin()); 
      i!=x.end(); 
      ++i) {
    y.push_back((*i)._);
  }
  return "/"+xju::format::join(y.begin(), y.end(), "/");
}

std::string str(AbsolutePath const& x, FileName const& y) throw()
{
  return str(x)+"/"+y._;
}
std::string str(std::pair<AbsolutePath, FileName> const& x) throw()
{
  return str(x.first, x.second);
}

std::string str(AbsolutePath const& x, DirName const& y) throw()
{
  return str(x)+"/"+y._+"/";
}
std::string str(std::pair<AbsolutePath, DirName> const& x) throw()
{
  return str(x.first, x.second);
}

std::string str(RelativePath const& x, FileName const& y) throw()
{
  if (x.size()) {
    return str(x)+"/"+y._;
  }
  else
  {
    return y._;
  }
}
std::string str(std::pair<RelativePath, FileName> const& x) throw()
{
  return str(x.first, x.second);
}

std::string str(FileName const& x) throw()
{
  return x._;
}

FileName basename(std::string const& x) throw()
{
  std::vector<DirName> const y = split_string(x);
  if (!y.size()) {
    return FileName("");
  }
  return FileName(y.back()._);
}

FileName basename(std::pair<AbsolutePath,FileName> const& x) throw()
{
  return x.second;
}

DirName basename(AbsolutePath const& x) throw()
{
  return *x.rbegin();
}

RelativePath relative_dirname(std::string const& x) /*throw(
  xju::Exception)*/
{
  try {
    if (x.size() && (x[0]=='/')) {
      std::ostringstream s;
      s << x << " starts with '/'";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    std::vector<DirName> const y = split_string(x);
    if (y.size()<1) {
      return RelativePath("");
    }
    
    return RelativePath(std::vector<DirName>(y.begin(), y.end()-1));
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "get directory part of " << x 
      << " assuming it is a relative path";
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}

AbsolutePath absolute_dirname(std::string const& x) /*throw(
  xju::Exception)*/
{
  try {
    if (!x.size() || (x[0]!='/')) {
      std::ostringstream s;
      s << x << " does not start with '/'";
      throw xju::Exception(s.str(), XJU_TRACED);
    }
    std::vector<DirName> const y = 
      split_string(std::string(x.begin()+1, x.end()));
    if (y.size()<1) {
      return AbsolutePath("/");
    }
    return AbsolutePath(std::vector<DirName>(y.begin(), y.end()-1));
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "get directory part of " << x 
      << " assuming it is an absolute path";
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}

std::pair<AbsolutePath, FileName> split(std::string const& x) /*throw(
  xju::Exception)*/
{
  try {
    if (x.size() && (x[0]=='/')) {
      return std::make_pair(absolute_dirname(x), basename(x));
    }
    return std::make_pair(working_dir()+relative_dirname(x), basename(x));
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "split " << x << " into absolute path and filename, from working "
      << "directory " << working_dir();
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}

std::pair<AbsolutePath, FileName> split(std::string const& x,
                                        xju::path::AbsolutePath const& working_dir) /*throw(
  xju::Exception)*/
{
  try {
    if (x.size() && (x[0]=='/')) {
      return std::make_pair(absolute_dirname(x), basename(x));
    }
    return std::make_pair(working_dir+relative_dirname(x), basename(x));
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "split " << x << " into absolute path and filename, from working "
      << "directory " << working_dir;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}

AbsolutePath splitdir(std::string const& x) /*throw(
  xju::Exception)*/
{
  try {
    if (x.size() && (x[0]=='/')) {
      return absolute_dirname(x)+RelativePath( {DirName(basename(x)._)});
    }
    return working_dir()+relative_dirname(x)+RelativePath(
      {DirName(basename(x)._)});
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "split " << x << " into absolute path and dirname, from working "
      << "directory " << working_dir();
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}

AbsolutePath splitdir(std::string const& x,
                      AbsolutePath const& working_dir) /*throw(
  xju::Exception)*/
{
  try {
    if (x.size() && (x[0]=='/')) {
      return absolute_dirname(x)+RelativePath( {DirName(basename(x)._)});
    }
    return working_dir+relative_dirname(x)+RelativePath(
      {DirName(basename(x)._)});
  }
  catch(xju::Exception& e) {
    std::ostringstream s;
    s << "split " << x << " into absolute path and dirname, from working "
      << "directory " << working_dir;
    e.addContext(s.str(), XJU_TRACED);
    throw;
  }
}

AbsolutePath dirname(
  std::pair<AbsolutePath, FileName> const& x) throw()
{
  return x.first;
}

AbsolutePath dirname(
  AbsolutePath const& x) throw()
{
  xju::assert_not_equal(x.size(),0);
  return AbsolutePath(
    std::vector<DirName>(x.begin(),xju::prev(x.end())));
}

std::pair<BaseName,Extension> split(FileName const& x) throw(){
  std::string const& v(x.value());
  if (v==""){
    return std::make_pair(BaseName(""),Extension(""));
  }
  std::string::const_reverse_iterator lastDot(
    std::find(v.rbegin(),v.rend(),'.'));
  if(lastDot==v.rend()||
     // handle filename ".y"
     lastDot==xju::prev(v.rend()))
  {
    return std::make_pair(BaseName(v),Extension(""));
  }
  size_t const lastDotOffset(v.rend()-lastDot-1);
  
  return std::make_pair(
    BaseName(std::string(v.begin(),v.begin()+lastDotOffset)),
    Extension(std::string(v.begin()+lastDotOffset,v.end())));
}


}
}
