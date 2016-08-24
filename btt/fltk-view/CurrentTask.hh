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
//    Display current task.
//
#ifndef _BTT_VIEW_CURRENTTASK_HH_
#define _BTT_VIEW_CURRENTTASK_HH_

#include <FL/Fl_Output.H>
#include <btt/WorkLog.hh>
#include <btt/Tasks.hh>
#include <xju/Observer.hh>
#include <xju/Optional.hh>
#include <memory>

namespace btt
{
    namespace view
    {
	class CurrentTask
	{
	public:
	    // Display current task summary in specified output filed, deriving
	    // it from WorkLog and Tasks.
	    // - tracks changes e.g. new current task selected
	    //
	    // pre: lifetime(output) > lifetime(this)
	    //      lifetime(worklog) > lifetime(this)
	    //      lifetime(tasks) > lifetime(this)
	    //
	    CurrentTask(Fl_Output& output,
			const WorkLog& workLog,
			const Tasks& tasks) throw():
		output_(output),
		workLog_(workLog),
		tasks_(tasks),
		entryAdded_(*this,
			    &CurrentTask::update,
			    workLog.entryAdded()),
		entryRemoved_(*this,
			      &CurrentTask::update,
			      workLog.lastEntryRemoved())
		
	    {
		update();
	    }

	private:
	    Fl_Output& output_;
	    const WorkLog& workLog_;
	    const Tasks&   tasks_;

	    //
	    // Set text describing current task.
	    //
	    void update() throw();
	    
	    xju::Observer<CurrentTask> entryAdded_;
	    xju::Observer<CurrentTask> entryRemoved_;
            std::unique_ptr<xju::Observer<CurrentTask> > currentTaskModified_;
	};
    }
}

#endif
