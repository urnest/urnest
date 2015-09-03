// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "InconsistentName.hh"

namespace xju
{
namespace snmp
{

InconsistentName::InconsistentName(Oid const& param, 
                                   const xju::Traced& trace) throw():
    InvalidParam("object "+param.toString()+
                 " could not be created this time eg another variable has an incompatible value",
                 param,
                 trace)
{
}


}
}

