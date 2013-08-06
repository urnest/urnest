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
#ifndef XJU_PATH_HH_
#define XJU_PATH_HH_

#include <xju/Tagged.hh>
#include <string>
#include <vector>
#include "xju/Exception.hh"

namespace xju
{
namespace path
{
class DirNameTag{};
typedef xju::Tagged<std::string, DirNameTag> DirName;

class RelativePath
{
public:
  explicit RelativePath(std::vector<DirName> const& x) throw():
    x_(x) {
  }
  explicit RelativePath(std::string x) throw(
    // x is not relative (ie starts with '/')
    xju::Exception);

  typedef std::vector<DirName>::const_iterator const_iterator;
  
  size_t size() const throw() {
    return x_.size();
  }
  
  const_iterator begin() const throw() {
    return x_.begin();
  }
  
  const_iterator end() const throw() {
    return x_.end();
  }
  
private:
  std::vector<DirName> x_;

  friend std::string str(RelativePath const& x) throw();
};

class AbsolutePath
{
public:
  // normalises, ie no component begin()..end() is '..', or '.', or ''
  explicit AbsolutePath(std::string const& x) throw(
    // x is not absolute, or
    // x has too many '..' to normalise
    xju::Exception);

  explicit AbsolutePath(std::vector<DirName> const& x) throw(
    // x has too many '..' to normalise
    xju::Exception);

  typedef std::vector<DirName>::const_iterator const_iterator;
  
  size_t size() const throw() {
    return x_.size();
  }
  
  const_iterator begin() const throw() {
    return x_.begin();
  }
  
  const_iterator end() const throw() {
    return x_.end();
  }
  
private:
  std::vector<DirName> x_;
  
  friend AbsolutePath operator+(AbsolutePath const& x, 
                                RelativePath const& y) throw(
                                  xju::Exception);
  friend bool operator==(AbsolutePath const& x,
                         AbsolutePath const& y) throw() {
    return x.x_ == y.x_;
  }
  
  friend std::ostream& operator<<(std::ostream& s,
                                  AbsolutePath const& x) throw();
};


// result is normalised
AbsolutePath operator+(AbsolutePath const& x, RelativePath const& y) throw(
  // y goes above root
  xju::Exception);

AbsolutePath root() throw();
AbsolutePath working_dir() throw();

// result has no trailing /
std::string str(AbsolutePath const& x) throw();

// result has no trailing /
// result is never "" (will be "." instead)
std::string str(RelativePath const& x) throw();

class FileNameTag{};
typedef xju::Tagged<std::string, FileNameTag> FileName;

std::string str(AbsolutePath const& x, FileName const& y) throw();
std::string str(std::pair<AbsolutePath, FileName> const& x) throw();

std::string str(RelativePath const& x, FileName const& y) throw();
std::string str(std::pair<RelativePath, FileName> const& x) throw();


// filename part of x
FileName basename(std::string const& x) throw();

// directory part of x, where x is filename with optional
// relative directory, e.g. relative_dirname("x/y")==RelativePath("x")
RelativePath relative_dirname(std::string const& x) throw(
  // x has absolute path
  xju::Exception);

// directory part of x, where x is filename with absolute directory,
// e.g. absolute_dirname("/x/y")==AbsolutePath("/x")
AbsolutePath absolute_dirname(std::string const& x) throw(
  // x has relative path only
  xju::Exception);

// split x into absolute path (using working dir if necessary) and
// file name
std::pair<AbsolutePath, FileName> split(std::string const& x) throw(
  // can't normalise
  xju::Exception);

}
}

#endif
