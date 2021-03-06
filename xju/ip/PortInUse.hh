// Copyright (c) 2017 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_IP_PORTINUSE_H
#define XJU_IP_PORTINUSE_H

#include "xju/Exception.hh"

namespace xju
{
namespace ip
{

class PortInUse : public xju::Exception
{
public:
  explicit PortInUse(xju::Exception const& x) throw():
      xju::Exception(x)
  {
  }
  explicit PortInUse(xju::Exception&& x) throw():
      xju::Exception(x)
  {
  }
};

  
}
}
#endif
