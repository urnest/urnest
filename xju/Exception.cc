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
//    Implementation for st_Exception.hh
//
static const char rcsid[] = "$RCSfile: Exception.cc,v $ $Revision: 1.7 $";
#include <xju/Exception.hh>


#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <xju/JoiningIterator.hh>
#include <sstream>

namespace
{
    std::string getText(
	const std::pair<std::string, xju::Traced>& x) throw()
    {
	return x.first;
    }
}

namespace xju
{
    Exception::Exception(const std::string& cause, const xju::Traced& trace) 
	throw():
	_cause(std::make_pair(cause, trace))
    {
    }
    Exception::Exception(const std::ostringstream& cause,
                         const xju::Traced& trace) throw():
	_cause(std::make_pair(cause.str(), trace))
    {
    }
    
    const std::pair<std::string, xju::Traced>& Exception::cause() const throw()
    {
	return _cause;
    }
    
    const std::vector<std::pair<std::string, xju::Traced> >& Exception::context()
	const throw()
    {
	return _context;
    }
    
    void Exception::addContext(const std::string& c,
			       const xju::Traced& t) throw()
    {
	_context.push_back(std::make_pair(c, t));
    }
    void Exception::addContext(const std::ostringstream& c,
			       const xju::Traced& t) throw()
    {
	_context.push_back(std::make_pair(c.str(), t));
    }

std::ostream& operator<<(std::ostream& s, const xju::Exception& e) throw()
{
    s << readableRepr(e, false);
    return s;
}

namespace
{
    std::string formatContext(
	const std::pair<std::string, xju::Traced>& x) throw()
    {
        std::ostringstream s;
        s << x.first
          << " ["
          << x.second.file()
          << ":"
          << x.second.line()
          << "]";
	return s.str();
    }
}

std::string readableRepr(const xju::Exception& e,
                         const bool includeTrace,
                         const bool asPhrase) throw()
{
    std::ostringstream s;
    s << (asPhrase?"failed to ":"Failed to ");
    
    std::transform(
        e.context().rbegin(),
        e.context().rend(),
        xju::JoiningIterator<std::string, std::string>(
            s,
            " because\nfailed to "),
        (includeTrace?formatContext:getText));
    
    s << " because" << std::endl
      << (includeTrace?formatContext:getText)(e.cause())
      << (asPhrase?"":".");
    return s.str();
}

}

