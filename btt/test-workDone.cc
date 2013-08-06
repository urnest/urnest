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


#include <unistd.h>
#include <xju/assert.hh>
#include <btt/WorkLog.hh>


void test1(const std::string& workLogFile) throw()
{
    ::unlink(workLogFile.c_str());
    
    btt::WorkLog l(workLogFile);
    
    const std::map<btt::TaskId, unsigned int> x(
	btt::workDone(l.rbegin(),
		      l.rend(),
		      xju::Time(0, 0),
		      xju::Time(10, 0)));
    xju::assert_equal(x.size(), 0U);
}

void test2(const std::string& workLogFile) throw()
{
    ::unlink(workLogFile.c_str());
    
    btt::WorkLog l(workLogFile);
    l.logWorkingOnTaskSince(btt::TaskId(1), xju::Time(5, 0));
    
    {
	const std::map<btt::TaskId, unsigned int> x(
	    btt::workDone(l.rbegin(),
			  l.rend(),
			  xju::Time(0, 0),
			  xju::Time(5, 0)));
	xju::assert_equal(x.size(), 0U);
    }
    
    {
	const std::map<btt::TaskId, unsigned int> x(
	    btt::workDone(l.rbegin(),
			  l.rend(),
			  xju::Time(0, 0),
			  xju::Time(8, 0)));
	xju::assert_equal(x.size(), 1U);
	xju::assert_equal((*x.begin()).first, btt::TaskId(1U));
	xju::assert_equal((*x.begin()).second, 3U);
    }
    {
	const std::map<btt::TaskId, unsigned int> x(
	    btt::workDone(l.rbegin(),
			  l.rend(),
			  xju::Time(6, 0),
			  xju::Time(8, 0)));
	xju::assert_equal(x.size(), 1U);
	xju::assert_equal((*x.begin()).first, btt::TaskId(1U));
	xju::assert_equal((*x.begin()).second, 2U);
    }
}


void test3(const std::string& workLogFile) throw()
{
    ::unlink(workLogFile.c_str());
    
    btt::WorkLog l(workLogFile);
    l.logWorkingOnTaskSince(btt::TaskId(1), xju::Time(5, 0));
    l.logWorkingOnTaskSince(btt::TaskId(2), xju::Time(8, 0));
    
    {
	const std::map<btt::TaskId, unsigned int> x(
	    btt::workDone(l.rbegin(),
			  l.rend(),
			  xju::Time(0, 0),
			  xju::Time(5, 0)));
	xju::assert_equal(x.size(), 0U);
    }
    
    {
	const std::map<btt::TaskId, unsigned int> x(
	    btt::workDone(l.rbegin(),
			  l.rend(),
			  xju::Time(6, 0),
			  xju::Time(8, 0)));
	xju::assert_equal(x.size(), 1U);
	xju::assert_equal((*x.begin()).first, btt::TaskId(1U));
	xju::assert_equal((*x.begin()).second, 2U);
    }
    {
	const std::map<btt::TaskId, unsigned int> x(
	    btt::workDone(l.rbegin(),
			  l.rend(),
			  xju::Time(8, 0),
			  xju::Time(10, 0)));
	xju::assert_equal(x.size(), 1U);
	xju::assert_equal((*x.begin()).first, btt::TaskId(2U));
	xju::assert_equal((*x.begin()).second, 2U);
    }
    {
	const std::map<btt::TaskId, unsigned int> x(
	    btt::workDone(l.rbegin(),
			  l.rend(),
			  xju::Time(6, 0),
			  xju::Time(11, 0)));
	xju::assert_equal(x.size(), 2U);
	xju::assert_equal((*x.begin()).first, btt::TaskId(1U));
	xju::assert_equal((*x.begin()).second, 2U);
	xju::assert_equal((*boost::next(x.begin())).first, btt::TaskId(2U));
	xju::assert_equal((*boost::next(x.begin())).second, 3U);
    }
    {
	const std::map<btt::TaskId, unsigned int> x(
	    btt::workDone(l.rbegin(),
			  l.rend(),
			  xju::Time(3, 0),
			  xju::Time(12, 0)));
	xju::assert_equal(x.size(), 2U);
	xju::assert_equal((*x.begin()).first, btt::TaskId(1U));
	xju::assert_equal((*x.begin()).second, 3U);
	xju::assert_equal((*boost::next(x.begin())).first, btt::TaskId(2U));
	xju::assert_equal((*boost::next(x.begin())).second, 4U);
    }

    l.logWorkingOnTaskSince(btt::TaskId(1), xju::Time(12, 0));
    {
	const std::map<btt::TaskId, unsigned int> x(
	    btt::workDone(l.rbegin(),
			  l.rend(),
			  xju::Time(3, 0),
			  xju::Time(14, 0)));
	xju::assert_equal(x.size(), 2U);
	xju::assert_equal((*x.begin()).first, btt::TaskId(1U));
	xju::assert_equal((*x.begin()).second, 5U);
	xju::assert_equal((*boost::next(x.begin())).first, btt::TaskId(2U));
	xju::assert_equal((*boost::next(x.begin())).second, 4U);
    }
}


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
	std::cerr << "usage: test-Controller <tempFile1>" 
		  << std::endl;
	return 1;
    }
    
    unsigned int n(0);
    
    test1(argv[1]); ++n;
    test2(argv[1]); ++n;
    test3(argv[1]); ++n;
    
    std::cout << "Pass - " << n << " steps" << std::endl;
    return 0;
}
