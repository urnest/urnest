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
//    Figure out how much time was spent on which tasks during the
//    specified period.
//
#ifndef _BTT_WORKDONE_HH_
#define _BTT_WORKDONE_HH_


#include <map>
#include <btt/TaskId.hh>
#include <xju/Time.hh>
#include <xju/Exception.hh>
#include <btt/WorkLog.hh>


namespace btt
{
    //
    // Report how much time was spent on which tasks during the
    // specified period.
    //
    // Note that where to is in the future, totals only include
    // the time "to now" on the current task (not any (future) time
    // between now and to.
    //
    // pre:  from <= to
    // post: result[i].second for all valid i
    //       result[i].second is the number of seconds spent on i
    //       during the period.
    //
    std::map<btt::TaskId, unsigned int> workDone(
	const btt::WorkLog::const_reverse_iterator& begin,
	const btt::WorkLog::const_reverse_iterator& end,
	const xju::Time& from,
	const xju::Time& to) throw(
	    xju::Exception);
}


#endif
