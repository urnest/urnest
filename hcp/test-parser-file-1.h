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
//    Task id type.
//
#ifndef _BTT_TASKID_HH_
#define _BTT_TASKID_HH_


#include <string>
#include <xju/Exception.hh>
#include <iostream>

namespace btt
{
    std::ostream& operator<<(std::ostream& s, const TaskId& x) throw();
}

class TaskId
{
};

class TaskId
{
};
std::ostream& operator<<(std::ostream& s, const TaskId& x) throw();

namespace btt
{
    class TaskId
    {
    };
    std::ostream& operator<<(std::ostream& s, const TaskId& x) throw();
}

namespace btt
{
    class TaskId
    {
    public:
	explicit TaskId(unsigned int value) throw():
	    value_(value)
	{
	}

	//
	// conversion to/from std::string
	//
	// post: result contains no new lines
	//
	std::string str() const throw();
	explicit TaskId(const std::string& value) throw(
	    //
	    // pre: value was not produced by str()
	    //
	    xju::Exception);
	
	bool operator==(const TaskId& x) const throw()
	{
	    return value_ == x.value_;
	}
	bool operator<(const TaskId& x) const throw()
	{
	    return value_ < x.value_;
	}
	
	std::string readableRepr() const throw();

	TaskId next() const throw()
	{
	    return TaskId(value_+1);
	}
	
    private:
	int value_;
    };
    
    std::ostream& operator<<(std::ostream& s, const TaskId& x) throw();
}


#endif
