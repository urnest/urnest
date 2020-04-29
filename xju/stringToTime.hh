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
//    Convert a string to a xju::Time. String examples are:
//
//       20020425 8:00am
//
//    String is assumed to represent a local time; result is GMT.
//
#ifndef _XJU_STRINGTOTIME_HH_
#define _XJU_STRINGTOTIME_HH_


#include <xju/Time.hh>
#include <xju/Exception.hh>
#include <string>


namespace xju
{
    xju::Time stringToTime(const std::string& s) /*throw(
	xju::Exception)*/;
}


#endif
