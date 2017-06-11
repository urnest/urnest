#ifndef _XJU_IP_HH
#define _XJU_IP_HH
// Copyright (c) 2015 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
// misc IP
//
#include <xju/Int.hh>
#include <stdint.h>
#include <iosfwd>

namespace xju
{
namespace ip
{
struct PortTag{};

// port number
typedef xju::Int<PortTag,uint16_t> Port;

namespace v4
{
struct AddressTag{};

// address (in host byte order)
typedef xju::Int<AddressTag,uint32_t> Address;

std::ostream& operator<<(std::ostream& s, Address const& a) throw();


}

}
}
#endif
