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
#include <btt/TaskId.hh>


#include <xju/format.hh>
#include <xju/stringToInt.hh>
#include <sstream>


namespace btt
{
    std::string TaskId::str() const throw()
    {
	return xju::format::int_(value_);
    }
    
    TaskId::TaskId(const std::string& value) throw(
	xju::Exception)
    {
	try
	{
	    value_ = xju::stringToInt(value);
	}
	catch(xju::Exception& e)
	{
	    std::ostringstream s;
	    s << "convert " << xju::format::quote(value)
	      << " to a task id";
	    e.addContext(s, XJU_TRACED);
	    throw;
	}
    }
    std::string TaskId::readableRepr() const throw()
    {
	return xju::format::int_(value_);
    }
    
    std::ostream& operator<<(std::ostream& s, const TaskId& x) throw()
    {
	return s << x.readableRepr();
    }
}
