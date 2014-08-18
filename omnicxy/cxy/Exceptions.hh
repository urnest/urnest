// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef CXY_EXCEPTIONS_H
#define CXY_EXCEPTIONS_H

#include <string>
#include <utility>

namespace cxy
{
template<class ExceptionBaseClass>
class Exceptions
{
public:
  class NoSuchObject : public ExceptionBaseClass
  {
  public:
    NoSuchObject(std::string const& s,
                 std::pair<std::string, unsigned int> const& src) throw():
        ExceptionBaseClass(s, src)
    {
    }
  };
  class WrongType : public ExceptionBaseClass
  {
  public:
    WrongType(std::string const& s,
              std::pair<std::string, unsigned int> const& src) throw():
        ExceptionBaseClass(s, src)
    {
    }
  };
};

  
}

#endif
