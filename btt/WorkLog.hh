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
//    Interface to (wrapper for) file containing 
//    log of periods tasks were worked on.
//
//    The file has no "breaks" i.e. must be working on a task
//    at any time. (Only task switches are recorded, so the
//    last entry identifies the task that is currently being
//    worked on.)
//
//    Not multi-thread-reentrant.
//
//  NOTES:
//
//    [1] The term "file" denotes the file itself (identity and
//        content, so "file' = file" means the file is not changed
//        in any way.
//
#ifndef _BTT_WORKLOG_HH_
#define _BTT_WORKLOG_HH_


#include <string>
#include <xju/Exception.hh>
#include <xju/NonCopyable.hh>
#include <utility>
#include <memory>
#include <xju/Time.hh>
#include <btt/TaskId.hh>
#include <xju/Event.hh>
#include <sys/types.h>
#include <unistd.h>
#include <xju/AutoFd.hh>

namespace btt
{
    class WorkLog : xju::NonCopyable
    {
    public:
	
	//
	// Wrap the specified work log file, creating it (empty) if
	// necessary.
	//
	// post: file' = file [1]
	//
	WorkLog(const std::string& fileName) throw(
	    //
	    // pre: file not readable
	    // post: file' = file [1]
	    //
	    xju::Exception);
	
	//
	// First is time started working on the task.
	// Second is which task.
	//
	typedef std::pair<xju::Time, TaskId> Entry;

	typedef Entry value_type;


	//
	// Record that user has been working on the
	// specified task since the specified time.
	//
	// post: *rbegin()' = Entry(since, task)
	//       entryAdded() has been triggered
	//       no other change
	//
	void logWorkingOnTaskSince(
	    const TaskId& task,
	    const xju::Time& since) throw(
		//
		// pre: since is less than (*rbegin()).first, or
		//      failed to write entry to task log
		// post: file' = file
		//
		xju::Exception);

	//
	// Event that is triggered whenever an entry
	// is added, just after adding the entry.
	//
	// post: lifetime(result) = lifetime(this)
	//
	const xju::Event& entryAdded() const throw();


	//
	// Remove the last entry from the log.
	//
	// pre:  rbegin() != rend()
	// post: rbegin()' == ++rbegin()
	//       all existing iterators valid except rbegin()
	//
	void pop_back() throw(
	    //
	    // pre: logWorkingOnTaskSince() has ever been called before, and
	    //      failed to remove last entry
	    // post: file' = file
	    //
	    xju::Exception);
	

	//
	// Event that is triggered whenever the last entry
	// is removed. The event is triggered *after* the
	// entry has already been removed.
	//
	// post: lifetime(result) = lifetime(this)
	//
	const xju::Event& lastEntryRemoved() const throw();
	

	struct const_reverse_iterator
	{
	private:
	    const_reverse_iterator(const std::string& fileName,
				   const int& fd,
				   const off_t offset) throw();

	public:
	    bool operator==(const_reverse_iterator i) const throw();
	    bool operator!=(const_reverse_iterator i) const throw()
	    {
		return !operator==(i);
	    }

	    WorkLog::Entry operator*() const throw(
		//
		// pre: file read failure
		// post: file' = file [1]
		//       *this' = *this
		//
		xju::Exception);

	    //
	    // pre: *this != rend()
	    //
	    const_reverse_iterator& operator++() throw(
		//
		// pre: file read failure
		// post: *this' = *this
		//       file' = file [1]
		//
		xju::Exception);
	    
	private:
	    const std::string fileName_;
	    const int& fd_;
	    off_t offset_;

	    friend class WorkLog;
	};

	const_reverse_iterator rbegin() const throw(
	    //
	    // pre: seek failure
	    //
	    xju::Exception);

	const_reverse_iterator rend() const throw();

	const std::string fileName_;
		
    private:
	xju::AutoFd fd_;
	xju::Event entryAdded_;
	xju::Event lastEntryRemoved_;
	
	std::auto_ptr<xju::AutoFd> wfd_;
    };
}


#endif
