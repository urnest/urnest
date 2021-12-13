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
class FileNameTag{};
typedef xju::Tagged<std::string, FileNameTag> FileName;

class DirNameTag{};
typedef xju::Tagged<std::string, DirNameTag> DirName;

class RelativePath
{
public:
  explicit RelativePath(std::vector<DirName> const& x) throw():
    x_(x) {
  }
  explicit RelativePath(std::string x) /*throw(
    // x is not relative (ie starts with '/')
    xju::Exception)*/;

  typedef std::vector<DirName>::const_iterator const_iterator;
  typedef std::vector<DirName>::const_reverse_iterator const_reverse_iterator;
  
  size_t size() const throw() {
    return x_.size();
  }
  
  const_iterator begin() const throw() {
    return x_.begin();
  }
  
  const_iterator end() const throw() {
    return x_.end();
  }
  
  const_reverse_iterator rbegin() const throw() {
    return x_.rbegin();
  }
  
  const_reverse_iterator rend() const throw() {
    return x_.rend();
  }
  
private:
  std::vector<DirName> x_;

  friend std::string str(RelativePath const& x) throw();

  friend bool operator<(RelativePath const& x, RelativePath const& y) throw()
  {
    return x.x_ < y.x_;
  }
  friend bool operator>(RelativePath const& x, RelativePath const& y) throw()
  {
    return y < x;
  }
  friend bool operator==(RelativePath const& x, RelativePath const& y) throw()
  {
    return !(y < x) && !(x < y);
  }
  friend bool operator!=(RelativePath const& x, RelativePath const& y) throw()
  {
    return (y < x) || (x < y);
  }
  friend bool operator<=(RelativePath const& x, RelativePath const& y) throw()
  {
    return (x < y) || (x==y);
  }
  friend bool operator>=(RelativePath const& x, RelativePath const& y) throw()
  {
    return (x > y) || (x==y);
  }
};

class AbsolutePath
{
public:
  // normalises, ie no component begin()..end() is '..', or '.', or ''
  explicit AbsolutePath(std::string const& x) /*throw(
    // x is not absolute, or
    // x has too many '..' to normalise
    xju::Exception)*/;

  // normalises, ie no component begin()..end() is '..', or '.', or ''
  explicit AbsolutePath(std::vector<DirName> const& x) /*throw(
    // x has too many '..' to normalise
    xju::Exception)*/;

  typedef std::vector<DirName>::const_iterator const_iterator;
  typedef std::vector<DirName>::const_reverse_iterator const_reverse_iterator;
  
  size_t size() const throw() {
    return x_.size();
  }
  
  const_iterator begin() const throw() {
    return x_.begin();
  }
  
  const_iterator end() const throw() {
    return x_.end();
  }
  
  const_reverse_iterator rbegin() const throw() {
    return x_.rbegin();
  }
  
  const_reverse_iterator rend() const throw() {
    return x_.rend();
  }
  
private:
  std::vector<DirName> x_;
  
  friend AbsolutePath operator+(AbsolutePath const& x, 
                                RelativePath const& y) /*throw(
                                  xju::Exception)*/;

  friend AbsolutePath operator+(AbsolutePath const& x, 
                                DirName const& y) /*throw(
                                  xju::Exception)*/;

  friend std::pair<AbsolutePath,FileName> operator+(
    AbsolutePath const& x, 
    FileName const& y) /*throw(
      xju::Exception)*/;

  friend std::ostream& operator<<(std::ostream& s,
                                  AbsolutePath const& x) throw();

  friend bool operator<(AbsolutePath const& x, AbsolutePath const& y) throw()
  {
    return x.x_ < y.x_;
  }
  friend bool operator>(AbsolutePath const& x, AbsolutePath const& y) throw()
  {
    return y < x;
  }
  friend bool operator==(AbsolutePath const& x, AbsolutePath const& y) throw()
  {
    return !(y < x) && !(x < y);
  }
  friend bool operator!=(AbsolutePath const& x, AbsolutePath const& y) throw()
  {
    return (y < x) || (x < y);
  }
  friend bool operator<=(AbsolutePath const& x, AbsolutePath const& y) throw()
  {
    return (x < y) || (x==y);
  }
  friend bool operator>=(AbsolutePath const& x, AbsolutePath const& y) throw()
  {
    return (x > y) || (x==y);
  }
};


// result is normalised
AbsolutePath operator+(AbsolutePath const& x, RelativePath const& y) /*throw(
  // y goes above root
  xju::Exception)*/;

AbsolutePath root() throw();
AbsolutePath working_dir() throw();

// result has no trailing /
std::string str(AbsolutePath const& x) throw();

// result has no trailing /
// result is never "" (will be "." instead)
std::string str(RelativePath const& x) throw();

class BaseNameTag{};
typedef xju::Tagged<std::string, BaseNameTag> BaseName; //excludes .

class ExtensionTag{};
typedef xju::Tagged<std::string, ExtensionTag> Extension; //includes .

typedef std::pair<AbsolutePath, FileName> AbsFile;

std::string str(AbsolutePath const& x, FileName const& y) throw();
std::string str(std::pair<AbsolutePath, FileName> const& x) throw();

std::string str(AbsolutePath const& x, DirName const& y) throw();
std::string str(std::pair<AbsolutePath, DirName> const& x) throw();

std::string str(RelativePath const& x, FileName const& y) throw();
std::string str(std::pair<RelativePath, FileName> const& x) throw();

std::string str(FileName const& x) throw();

// filename part of x
FileName basename(std::string const& x) throw();

// filename part of x
FileName basename(std::pair<AbsolutePath,FileName> const& x) throw();

// last directory x
DirName basename(AbsolutePath const& x) throw();

// directory part of x, where x is filename with optional
// relative directory, e.g. relative_dirname("x/y")==RelativePath("x")
RelativePath relative_dirname(std::string const& x) /*throw(
  // x has absolute path
  xju::Exception)*/;

// directory part of x, where x is filename with absolute directory,
// e.g. absolute_dirname("/x/y")==AbsolutePath("/x")
AbsolutePath absolute_dirname(std::string const& x) /*throw(
  // x has relative path only
  xju::Exception)*/;

// split x into absolute path (using working dir if necessary) and
// file name
std::pair<AbsolutePath, FileName> split(std::string const& x) /*throw(
  // can't normalise
  xju::Exception)*/;

std::pair<AbsolutePath, FileName> split(std::string const& x,
                                        xju::AbsolutePath const& working_dir) /*throw(
  // can't normalise
  xju::Exception)*/;

// split x into absolute path (using working dir if necessary)
AbsolutePath splitdir(std::string const& x) /*throw(
  // can't normalise
  xju::Exception)*/;

// parent directory of x
// post: result==x.first
AbsolutePath dirname(
  std::pair<AbsolutePath, FileName> const& x) throw();

// parent directory of x
// pre: x.size()!=0
AbsolutePath dirname(
  AbsolutePath const& x) throw();

// split FileName into BaseName and Extension
// - split(FileName("x")) == {BaseName("x"),Extension("")}
// - split(FileName(".y")) == {BaseName(".y"),Extension("")}
// - split(FileName(".y.txt")) == {BaseName(".y"),Extension(".txt")}
// - split(FileName("y.tar.gz")) == {BaseName("y.tar"),Extension(".gz")}
std::pair<BaseName,Extension> split(FileName const& x) throw();

}
}

#endif
