//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 1997-2007 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/stringToInt.hh>


#include <xju/format.hh>
#include <stdlib.h>
#include <limits.h>
#include <sstream>

namespace xju
{
    int stringToInt(const std::string& value) throw(xju::Exception)
    {
	try
	{
	    if (value.size() == 0)
	    {
		std::ostringstream cause;
		cause << xju::format::quote(value) << " is null";
		throw xju::Exception(cause, XJU_TRACED);
	    }
	    char* p(0);
	    const long x(strtol(value.c_str(), &p, 10));
	    if (*p != 0)
	    {
		std::ostringstream cause;
		cause << "character " << (p - value.c_str() + 1)
		      << " ('" << (*p) << "') of "
		      << xju::format::quote(value) 
		      << " unexpected";
		throw xju::Exception(cause, XJU_TRACED);
	    }
	    const int result(x);
	    if ((long)result != x)
	    {
		std::ostringstream cause;
		cause << x << " is too large (maximum allowed integer is "
		      << INT_MAX
		      << ")";
		throw xju::Exception(cause, XJU_TRACED);
	    }
	    return result;
	}
	catch(xju::Exception& e)
	{
	    std::ostringstream s;
	    s << "convert " << xju::format::quote(value) << " to an integer";
	    e.addContext(s, XJU_TRACED);
	    throw;
	}
    }
}
