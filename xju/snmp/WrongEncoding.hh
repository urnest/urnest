// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_SNMP_WRONGENCODING_H
#define XJU_SNMP_WRONGENCODING_H

#include "xju/snmp/InvalidParam.hh"

namespace xju
{
namespace snmp
{
// exception corresponding to RFC 1905 error-status
class WrongEncoding : public InvalidParam
{
public:
  WrongEncoding(Oid const& param, const xju::Traced& trace) throw();
};

}
}

#endif

