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
#include <btt/workDone.hh>


#include <sstream>
#include <xju/formatTime.hh>
#include <boost/optional.hpp>
#include <utility>
#include <xju/Time.hh>
#include <map>
#include "xju/MicroSeconds.hh"


namespace
{
    boost::optional<xju::MicroSeconds> overlap(
	const std::pair<xju::Time, xju::Time>& a,
	const std::pair<xju::Time, xju::Time>& b) throw()
    {
	const xju::Time x(std::max(a.first, b.first));
	const xju::Time y(std::min(a.second, b.second));
	if (x < y)
	{
	    return boost::optional<xju::MicroSeconds>(y - x);
	}
	else
	{
	    return boost::optional<xju::MicroSeconds>();
	}
    }
}


namespace btt
{
    std::map<btt::TaskId, unsigned int> workDone(
	const btt::WorkLog::const_reverse_iterator& begin,
	const btt::WorkLog::const_reverse_iterator& end,
	const xju::Time& from,
	const xju::Time& to) throw(
	    xju::Exception)
    {
	try
	{
	    std::map<btt::TaskId, unsigned int> result;
	    if (begin == end)
	    {
		return result;
	    }

	    const boost::optional<xju::MicroSeconds> o(
		overlap(std::make_pair(from, to),
			std::make_pair(
			    (*begin).first,
			    xju::Time::now())));
	    if (o)
	    {
		const std::map<btt::TaskId, unsigned int>::iterator j(
		    result.insert(
			std::pair<const btt::TaskId, unsigned int>(
			    (*begin).second,
			    0U)).first);
		(*j).second += (*o).value()/1000000;
	    }
	    
	    for(btt::WorkLog::const_reverse_iterator i(begin);
		i != end && (*i).first >= from;
		++i)
	    {
		btt::WorkLog::const_reverse_iterator next(
		    boost::next(i));
		if (next != end)
		{
		    const boost::optional<xju::MicroSeconds> o(
			overlap(std::make_pair(from, to),
				std::make_pair(
				    (*next).first,
				    (*i).first)));
		    if (o)
		    {
			const std::map<btt::TaskId, unsigned int>::iterator j(
			    result.insert(
				std::pair<const btt::TaskId, unsigned int>(
				    (*next).second,
				    0U)).first);
			(*j).second += (*o).value()/1000000;
		    }
		}
	    }
	    return result;
	}
	catch(xju::Exception& e)
	{
	    std::ostringstream s;
	    s << "summarise work done from "
	      << xju::formatTime(from)
	      << " to "
	      << xju::formatTime(to);
	    e.addContext(s, XJU_TRACED);
	    throw;
	}
    }
}
