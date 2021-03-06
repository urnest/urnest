// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "NotWritable.hh"

namespace xju
{
namespace snmp
{

NotWritable::NotWritable(Oid const& param, const xju::Traced& trace) throw():
    InvalidParam("server has no object "+param.toString()+
                 " or that object is not writable",
                 param,
                 trace)
{
}


}
}

