//     -*- mode: c++ ; c-file-style: "osse" ;  -*-
//
// Copyright (c) 2007 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    Represents an exception of non-specific type.
//
// Example:
//
//   Here is a function that demonstrates generating and
//   adding context to an exception.
//
//   int openFileReadOnly(const string& fileName) throw(xju::Exception)
//   {
//      try
//      {
//         const int result(open(fileName.c_str(), O_RDONLY));
//         if (result == -1)
//         {
//            std::ostringstream cause;
//            cause << "open(2) failed, errno = " << errno;
//            throw xju::Exception(cause, XJU_TRACED);
//         }
//         return result;
//      }
//      catch(xju::Exception& e)
//      {
//         std::ostringstream context;
//         context << "open file " << fileName << " read-only, from directory "
//                 << getwd();
//         e.addContext(context, XJU_TRACED);
//         throw;
//      }
//   }
//
// See Also:
//
#ifndef _XJU_EXCEPTION_HH_
#define _XJU_EXCEPTION_HH_


#include <vector>
#include <string>
#include <utility>
#include <xju/Traced.hh>
#include <iostream>
#include <sstream>

namespace xju
{
    class Exception
    {
    public:
	//
	// Construct with specified cause and empty context.
	//
	// Note that the trace is typically supplied supplied using
	// the XJU_TRACED macro, e.g.:
	//
	//    std::ostringstream cause;
	//    cause << "open(2) failed, errno = " << errno;
	//    throw xju::Exception(cause, XJU_TRACED);
	//
	// post: cause() = cause
	//       context().size() = 0
	//
	Exception(const std::string& cause, const xju::Traced& trace) throw();
	Exception(const std::ostringstream& cause,
                  const xju::Traced& trace) throw();
	
	//
	// Report cause.
	//
	const std::pair<std::string, xju::Traced>& cause() const throw();
	
	//
	// Report context.
	//
	const std::vector<std::pair<std::string, xju::Traced> >& context() const
	    throw();
	
	//
	// Add specified context.
	//
	// post: cause = cause@pre
	//       context.size() = context.size()@pre + 1
	//       context.back() = make_pair(c, trace)
	//       context[0..context.size()-1] = context@pre
	//
	void addContext(const std::string& c,
                        const xju::Traced& trace) throw();
	void addContext(const std::ostringstream& c,
                        const xju::Traced& trace) throw();
	
	virtual ~Exception() throw() {}
	
    private:
	std::pair<std::string, xju::Traced> _cause;
	std::vector<std::pair<std::string, xju::Traced> > _context;
    };
//
// A human readable representation:
//
//   - multi-line
//   - last line not terminated by new-line
//   - contains cause and context strings, but not file and line number
//     information
//   - forms an english sentence
//
// (This could be used to present the exception to non-programmers, for
// whom the file and line number information is not useful. It should
// not be used for logging, since it omits file and line number information.)
//
// pre: e.context().size() > 0
//
std::ostream& operator<<(std::ostream& s, const xju::Exception& e) throw();


//
// Format as above where !includeTrace and !asPhrase, otherwise:
// - include trace (files and line numbers) where includeTrace
// - format as a phrase (initial lower-case, no trailing full-stop) where
//   asPhrase
std::string readableRepr(const xju::Exception& e,
                         const bool includeTrace = false,
                         const bool asPhrase = false) throw();
};


#endif
