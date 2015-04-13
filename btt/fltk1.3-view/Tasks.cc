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
#include <xju/formatTime.hh>
#include <xju/prev.hh>


namespace btt
{
    namespace view
    {
	Tasks::Tasks(Fl_Browser& widget,
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
            // const char *labels[] = {"Id",
            //                         "Started",
            //                         "Spent Hrs",
            //                         "Description",
            //                         0};
	    //widget.column_labels(labels);
            static int const widths[]   = {30, 70, 70, 0};
            widget.column_widths(widths);

            
	    btt::Tasks::const_iterator i;
	    xju::Ring<Task*>* successor(&registry_);
	    
	    for(i = tasks.end(); i != tasks.begin(); --i)
	    {
		btt::Tasks::const_iterator j(xju::prev(i));
		new Task(this, j, *successor, widget_);
		successor = &successor->prev();
	    }
            widget_.when(FL_WHEN_RELEASE|FL_WHEN_NOT_CHANGED);
	}

    }
}
