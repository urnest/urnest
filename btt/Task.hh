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
//    Represents a (project) Task, e.g. a HFMP project task
//    like "review HOC SRS".
//
#ifndef _BTT_TASK_HH_
#define _BTT_TASK_HH_


#include <xju/Notifying.hh>
#include <btt/TaskId.hh>
#include <boost/optional.hpp>
#include <xju/Time.hh>
#include <functional>


namespace btt
{
    class Task
    {
    public:
	Task(const TaskId& id,
	     const std::string description,
	     const unsigned int secondsSpentOnThisTask,
	     const boost::optional<xju::Time>& started) throw():
	    id_(id),
	    description_(description),
	    secondsSpentOnThisTask_(secondsSpentOnThisTask),
	    started_(started)
	{
	}

	const TaskId id_;
	TaskId id() const throw() 
	{
	    return id_;
	}
	
	xju::Notifying<std::string> description_;
	xju::Notifying<unsigned int> secondsSpentOnThisTask_;
	xju::Notifying<boost::optional<xju::Time> > started_;

	class HasId : public std::unary_function<Task, bool>
	{
	public:
	    HasId(const btt::TaskId& id) throw():
		id_(id)
	    {
	    }
	    bool operator()(const btt::Task& task) const throw()
	    {
		return task.id_ == id_;
	    }
	    const btt::TaskId id_;
	};
    };
}

#endif
