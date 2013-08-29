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
#include <btt/fltk1.3-view/Tasks.hh>


#include <xju/Ring.hh>
#include <xju/format.hh>
#include <xju/Observer.hh>
#include <boost/utility.hpp>
#include <xju/formatTime.hh>


namespace btt
{
    namespace view
    {
	Tasks::Tasks(fltk::Browser& widget,
                     const btt::Tasks& tasks) throw():
	    selectedTask_(tasks.end()),
            widget_(widget),
	    tasks_(tasks),
	    registry_(0),
	    taskAddedAtEnd_(*this,
			    &Tasks::taskAddedAtEnd,
			    tasks.newPredecessor(tasks.end())),
            tasksCallback_(widget, *this, &Tasks::tasksCallback)
	{
            const char *labels[] = {"Id",
                                    "Started",
                                    "Spent Hrs",
                                    "Description",
                                    0};
            int const widths[]   = {30, 70, 70, -1, 0};

	    widget.column_labels(labels);
            widget.column_widths(widths);
            
	    btt::Tasks::const_iterator i;
	    xju::Ring<Task*>* successor(&registry_);
	    
	    for(i = tasks.end(); i != tasks.begin(); --i)
	    {
		btt::Tasks::const_iterator j(boost::prior(i));
		new Task(this, j, *successor, widget_);
		successor = &successor->prev();
	    }
            widget_.when(fltk::WHEN_RELEASE|fltk::WHEN_NOT_CHANGED);
	}

    }
}
