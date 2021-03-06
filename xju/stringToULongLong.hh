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
//    Convert a string to an unsigned long long integer.
//
#ifndef _XJU_STRINGTOULONGLONG_HH_
#define _XJU_STRINGTOULONGLONG_HH_


#include <string>
#include <xju/Exception.hh>


namespace xju
{
    //
    // Convert specified string to an unsigned long long integer.
    //
    unsigned long long stringToULongLong(const std::string& value,
                                   unsigned int base = 10) /*throw(
	//
	// pre: string = "", or
	//      integer overflow, or
	//      strtoul(3) failure
	//
	xju::Exception)*/;
}


#endif
