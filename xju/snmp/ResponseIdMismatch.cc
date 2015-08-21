// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "ResponseIdMismatch.hh"
#include "xju/format.hh"

namespace xju
{
namespace snmp
{

ResponseIdMismatch::ResponseIdMismatch(RequestId const got,
                                       RequestId const expected,
                                       xju::Traced const& trace) throw():
    xju::Exception("expected response with id "+xju::format::str(expected)+
                   " but got response of id "+xju::format::str(got),
                   trace),
    got_(got),
    expected_(expected) 
{
}


}
}

