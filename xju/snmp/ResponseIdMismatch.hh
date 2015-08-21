// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_RESPONSEIDMISMATCH_H
#define XJU_SNMP_RESPONSEIDMISMATCH_H

#include "xju/Exception.hh"
#include "xju/snmp/RequestId.hh"

namespace xju
{
namespace snmp
{
class ResponseIdMismatch : public xju::Exception
{
public:
  ResponseIdMismatch(RequestId const got,
                     RequestId const expected,
                     xju::Traced const& trace) throw();

  RequestId got_;
  RequestId expected_;
};
  


}
}

#endif

