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
//    Convert a string to an integer.
//
#ifndef _XJU_STRINGTOINT_HH_
#define _XJU_STRINGTOINT_HH_


#include <string>
#include <xju/Exception.hh>


namespace xju
{
    //
    // Convert specified string to an integer.
    //
    int stringToInt(const std::string& value) /*throw(
	//
	// pre: string = "", or
	//      integer overflow, or
	//      strtol(3) failure
	//
	xju::Exception)*/;
}


#endif
