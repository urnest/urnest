//     -*- mode: c++ ; c-file-style: "osse" -*-
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
//    Convert a string to double precision float.
//
#ifndef _XJU_STRINGTODOUBLE_HH_
#define _XJU_STRINGTODOUBLE_HH_


#include <string>
#include <xju/Exception.hh>


namespace xju
{
    namespace util
    {
	//
	// Convert specified string to an double.
	//
	double stringToDouble(const std::string& value) throw(
	    //
	    // pre: string = "", or
	    //      strtod(3) failure
	    //
	    xju::Exception);
    }
}

	
#endif
