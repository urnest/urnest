// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_REQUESTTYPEMISMATCH_H
#define XJU_SNMP_REQUESTTYPEMISMATCH_H
#include "xju/Exception.hh"
#include <stdint.h>

namespace xju
{
namespace snmp
{

class RequestTypeMismatch : public xju::Exception
{
public:
  RequestTypeMismatch(uint8_t const got,
                      uint8_t const expected,
                      xju::Traced const& trace) throw();
};

}
}

#endif

