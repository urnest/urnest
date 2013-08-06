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
//    Format xju::Time into various readable forms.
//
#ifndef _XJU_FORMATTIME_HH_
#define _XJU_FORMATTIME_HH_


#include <xju/Time.hh>
#include <string>
#include <xju/Exception.hh>


namespace xju
{
    //
    // Result includes date and time.
    //
    // pre: format is as allowed by strftime(3)
    //      (Cygwin only: BTT_TZ environment variable is a float
    //      specifying the number of hours ahead of GMT of the
    //      local timezone.)
    //
    std::string formatTime(
	const xju::Time& t,
	const std::string format = std::string("%c")) throw(
	    xju::Exception);
}


#endif
