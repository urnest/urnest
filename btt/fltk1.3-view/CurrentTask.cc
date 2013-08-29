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
#include <btt/fltk-view/CurrentTask.hh>


#include <algorithm>
#include <xju/assert.hh>
#include <sstream>
#include <xju/formatTime.hh>
#include <string>
#include <btt/Task.hh>


namespace btt
{
    namespace view
    {
	void CurrentTask::update() throw()
	{
	    if (workLog_.rbegin() == workLog_.rend())
	    {
                currentTaskModified_.reset();
		output_.value("Not currently working on any task.");
                output_.redraw();
	    }
	    else
	    {
		const std::string dayToday(
		    xju::formatTime(xju::Time::now(), "%A"));
		
		const TaskId x((*workLog_.rbegin()).second);
		const xju::Time since((*workLog_.rbegin()).first);
		const Tasks::const_iterator i(
		    std::find_if(tasks_.begin(),
				 tasks_.end(),
				 Task::HasId(x)));
		xju::assert_not_equal(i, tasks_.end());
		
                currentTaskModified_ = 
                    std::auto_ptr<xju::Observer<CurrentTask> >(
                        new xju::Observer<CurrentTask>(
                            *this,
                            &CurrentTask::update,
                            (*i).description_.m_changed));
                    
		std::ostringstream s;
		s << "Current (since ";
		if (xju::formatTime(since, "%A") != dayToday)
		{
		    s << xju::formatTime(since, "%I:%M%p %A");
		}
		else
		{
		    s << xju::formatTime(since, "%I:%M%p");
		}
		s << "): "
		  << (*i).description_.value();
		output_.value(s.str().c_str());
                output_.redraw();
	    }
	}
    }
}
