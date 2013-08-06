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
//    test for CurrentTask.hh
//
//    REVISIT: automate
//
#include <btt/fltk-view/CurrentTask.hh>

#include <iostream>
#include <fltk/Window.h>
#include <fltk/Output.h>
#include <fltk/run.h>
#include <xju/assert.hh>

namespace
{
    //
    // post: !isspace(result)
    //
    char getResponse() throw()
    {
	char c;
	do
	{
	    std::cin.get(c);
	}
	while(std::isspace(c));
	return c;
    }

    class SomeTasks : public btt::Tasks
    {
    public:
	SomeTasks() throw()
	{
	    insert(
		end(),
		btt::Task(
		    btt::TaskId(1),
		    "desc1",
		    0,
		    boost::optional<xju::Time>()));
	    insert(
		end(),
		btt::Task(
		    btt::TaskId(2),
		    "desc2",
		    0,
		    boost::optional<xju::Time>()));
	}
    };
}

class UI {
public:
    UI() throw():
	w(310, 180),
	o((w.begin(), 0), 0, 300, 20, "") 
    {
	w.end();
	w.show();
    }
    void run() throw()
    {
	fltk::run();
    }
    fltk::Window w;
    fltk::Output o;
};

void test1(int argc, char* argv[],
	   const std::string& workLogFileName)
{
    ::unlink(workLogFileName.c_str());
    SomeTasks tasks;
    
    btt::WorkLog workLog(workLogFileName);
    
    std::cout << "verify not working on any task" << std::flush;
    
    UI ui;
    btt::view::CurrentTask view(ui.o, workLog, tasks);
    ui.run();

    xju::assert_equal(getResponse(), 'y');
}
    
void test2(int argc, char* argv[],
	   const std::string& workLogFileName)
{
    ::unlink(workLogFileName.c_str());
    SomeTasks tasks;
    
    btt::WorkLog workLog(workLogFileName);
    workLog.logWorkingOnTaskSince(
	btt::TaskId(2),
	xju::Time(0, 0));

    std::cout << "verify working on desc2" << std::flush;

    UI ui;
    btt::view::CurrentTask view(ui.o, workLog, tasks);
    ui.run();

    xju::assert_equal(getResponse(), 'y');
}
    
    
void test3(int argc, char* argv[],
	   const std::string& workLogFileName)
{
    ::unlink(workLogFileName.c_str());
    SomeTasks tasks;
    
    btt::WorkLog workLog(workLogFileName);
    workLog.logWorkingOnTaskSince(
	btt::TaskId(2),
	xju::Time(0, 0));

    std::cout << "verify working on desc1" << std::flush;

    UI ui;
    btt::view::CurrentTask view(ui.o, workLog, tasks);
    workLog.logWorkingOnTaskSince(
	btt::TaskId(1),
	xju::Time(10, 0));
    ui.run();

    xju::assert_equal(getResponse(), 'y');
}
    
    
void test4(int argc, char* argv[],
	   const std::string& workLogFileName)
{
    ::unlink(workLogFileName.c_str());
    SomeTasks tasks;
    
    btt::WorkLog workLog(workLogFileName);
    workLog.logWorkingOnTaskSince(
	btt::TaskId(2),
	xju::Time(0, 0));

    std::cout << "verify not working on any task" << std::flush;

    UI ui;
    btt::view::CurrentTask view(ui.o, workLog, tasks);
    workLog.pop_back();
    ui.run();
    
    xju::assert_equal(getResponse(), 'y');
}
    
    
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
	std::cerr << "specify temporary file name as parameter"
		  << std::endl;
	return 1;
    }
    unsigned int n(0);

    test1(argc, argv, argv[1]); ++n;
    test2(argc, argv, argv[1]); ++n;
    test3(argc, argv, argv[1]); ++n;
    test4(argc, argv, argv[1]); ++n;
    
    std::cout << "Pass - " << n << " steps" << std::endl;
    return 0;
}

