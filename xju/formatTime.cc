//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 2003
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/formatTime.hh>


#include <time.h>
#include <xju/format.hh>
#include <sstream>


namespace xju
{
    namespace
    {
#ifdef __MINGW32__
        struct tm* localtime_r(const time_t* t, struct tm* result)
        {
            // REVISIT: need serialisation
            *result = *::localtime(t);
            return result;
        }
#endif
    }
    
        
    std::string formatTime(
	const xju::Time& t,
	const std::string format) throw(
	    xju::Exception)
    {
	try
	{
	    const time_t secs(t.getSecs());
	    char s[128];
	    struct tm x;
	    localtime_r(&secs, &x);
	    strftime(s, sizeof(s), format.c_str(), &x);
	    return std::string(s);
	}
	catch(xju::Exception& e)
	{
	    std::ostringstream s;
	    s << "format time {"
	      << t.getSecs()
	      << ", "
	      << t.getUsecs()
	      << "} using format string "
	      << xju::format::quote(format);
	    e.addContext(s, XJU_TRACED);
	    throw;
	}
    }
}
