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
#include <btt/WorkLog.hh>


#include <iostream>
#include <xju/Observer.hh>
#include <xju/Event.hh>
#include <xju/assert.hh>
#include <btt/TaskId.hh>
#include <stdlib.h>
#include <sstream>
#include <xju/stat.hh>
#include <xju/Time.hh>
#include <xju/formatTime.hh>
#include <xju/format.hh>

class EventCounter
{
public:
    EventCounter(const xju::Event& e) throw():
	count_(0),
	happened_(*this, &EventCounter::happened, e)
    {
    }
    void happened() throw()
    {
	++count_;
    }
    unsigned int count_;

    xju::Observer<EventCounter> happened_;
};


void test1(const std::string& fileName)
{
    {
	btt::WorkLog l(fileName);
	
	EventCounter added(l.entryAdded());
	EventCounter removed(l.lastEntryRemoved());
	
	xju::assert_equal(l.rbegin(), l.rend());
	
	l.logWorkingOnTaskSince(btt::TaskId(4), xju::Time(1, 100));
	
	xju::assert_equal(added.count_, 1U);
	xju::assert_equal(removed.count_, 0U);
	xju::assert_not_equal(l.rbegin(), l.rend());
	xju::assert_equal((*l.rbegin()).second, btt::TaskId(4));
	xju::assert_equal((*l.rbegin()).first, xju::Time(1, 100));
	
	btt::WorkLog::const_reverse_iterator i(l.rbegin());
	++i;
	xju::assert_equal(i, l.rend());
    }
    {
	btt::WorkLog l(fileName);
	
	EventCounter added(l.entryAdded());
	EventCounter removed(l.lastEntryRemoved());
	
	xju::assert_not_equal(l.rbegin(), l.rend());
	xju::assert_equal((*l.rbegin()).second, btt::TaskId(4));
	xju::assert_equal((*l.rbegin()).first, xju::Time(1, 100));
	
	btt::WorkLog::const_reverse_iterator i(l.rbegin());
	++i;
	xju::assert_equal(i, l.rend());

	l.pop_back();

	xju::assert_equal(l.rbegin(), l.rend());
	
	xju::assert_equal(added.count_, 0U);
	xju::assert_equal(removed.count_, 1U);
    }
}


void test2(const std::string& fileName)
{
    btt::WorkLog l(fileName);
    
    EventCounter added(l.entryAdded());
    EventCounter removed(l.lastEntryRemoved());
    
    xju::assert_equal(l.rbegin(), l.rend());
    
    l.logWorkingOnTaskSince(btt::TaskId(4), xju::Time(1, 100));
    l.logWorkingOnTaskSince(btt::TaskId(5), xju::Time(2, 200));
    
    xju::assert_equal(added.count_, 2U);
    xju::assert_equal(removed.count_, 0U);
    xju::assert_not_equal(l.rbegin(), l.rend());
    btt::WorkLog::const_reverse_iterator i(l.rbegin());

    xju::assert_equal((*i).second, btt::TaskId(5));
    xju::assert_equal((*i).first, xju::Time(2, 200));
    ++i;
    xju::assert_equal((*i).second, btt::TaskId(4));
    xju::assert_equal((*i).first, xju::Time(1, 100));

    l.pop_back();
    xju::assert_equal((*i).second, btt::TaskId(4));
    xju::assert_equal((*i).first, xju::Time(1, 100));

    l.pop_back();
    xju::assert_equal(l.rbegin(), l.rend());
}

void test3(const std::string& fileName)
{
    // can't open file
    try
    {
	btt::WorkLog l("/dev/null/fred");
	abort();
    }
    catch(const xju::Exception& e)
    {
#ifdef __MINGW32__
	xju::assert_equal(
	    readableRepr(e),
	    std::string("Failed to wrap work log file /dev/null/fred because\n"
			"failed to open work log file /dev/null/fred "
			"read-only because\n"
			"open system call failed, errno = 2."));
#else
	xju::assert_equal(
	    readableRepr(e),
	    std::string("Failed to wrap work log file /dev/null/fred because\n"
			"failed to open work log file /dev/null/fred "
			"read-only because\n"
			"open system call failed, errno = 20."));
#endif
    }

    btt::WorkLog l(fileName);
    xju::syscall(xju::chmod, XJU_TRACED)(fileName.c_str(), 0444);

    const xju::Time tt(xju::Time::now());
    const std::string tts(xju::formatTime(tt));
    
    try
    {
	l.logWorkingOnTaskSince(btt::TaskId(4), tt);
	abort();
    }
    catch(const xju::Exception& e)
    {
	xju::assert_equal(
	    readableRepr(e),
	    std::string(
		"Failed to record that you have been working on task with "
		"id 4 since ") + tts + std::string(" because\n"
		"failed to open work log file " + fileName + 
                " for writing because\n"
		"open system call failed, errno = 13."));
	xju::syscall(xju::chmod, XJU_TRACED)(fileName.c_str(), 0644);
    }
}


//
// Create a temporary file name (name only; no path).
//
// Assumes program is only one running in a clean directory.
// Not thread safe.
//
// File is deleted by destructor.
//
class TempFile
{
public:
    TempFile():
	name_(mkName(next_))
    {
    }
    std::string name() const throw()
    {
	return name_;
    }
    ~TempFile() throw()
    {
	::unlink(name_.c_str());
    }
    
private:
    const std::string name_;
    static int next_;

    static const std::string mkName(int& next) throw()
    {
	return std::string("tmp_")+xju::format::int_(next++);
    }
};
int TempFile::next_(1);


int main(int argc, char* argv[])
{
    TempFile f1;

    unsigned int n(0);

    try
    {
	test1(f1.name()); ++n;
	test2(f1.name()); ++n;
	test3(f1.name()); ++n;
	
	std::cout << "Pass - " << n << " steps" << std::endl;
	return 0;
    }
    catch(xju::Exception& e)
    {
	std::ostringstream s;
	s << "do step " << (n+1);
	e.addContext(s, XJU_TRACED);
	std::cerr << readableRepr(e) << std::endl;
	return 1;
    }
}
