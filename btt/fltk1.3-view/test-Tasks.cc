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
// REVISIT: automate
#include <btt/fltk-view/Tasks.hh>


#include <iostream>
#include <xju/assert.hh>
#include <fltk/Window.h>
#include <fltk/Browser.h>
#include <fltk/run.h>


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
}

class UI {
public:
    UI() throw():
	w(310, 180),
	o((w.begin(), 0), 0, 300, 220, "") 
    {
	w.end();
	w.show();
    }
    void run() throw()
    {
	fltk::run();
    }
    void show() throw()
    {
        w.show();
    }
    fltk::Window w;
    fltk::Browser o;
};

void test1(const btt::Tasks& tasks,
	   btt::view::Tasks& view,
           UI& ui)
{
    std::cout << "verify list is empty (y for yes)" << std::flush;

    ui.show();
    ui.run();
    
    xju::assert_equal(getResponse(), 'y');
}

class Latch
{
public:
    explicit Latch() throw():
        value_(false)
    {
    }
    void latch() throw()
    {
        value_ = true;
    }
    bool value_;
};


void test2(btt::Tasks& tasks,
	   btt::view::Tasks& view,
           UI& ui)
{
    tasks.insert(tasks.end(),
		 btt::Task(btt::TaskId(1),
			   "task 1 desc",
			   13*60*60,
			   boost::optional<xju::Time>(xju::Time(1, 0))));
    
    std::cout << "verify list has 1, <start>, task 1 desc, 13.00 (y for yes)"
	      << std::flush;
    ui.show();
    ui.run();

    xju::assert_equal(getResponse(), 'y');

    Latch l1;
    xju::Observer<Latch> o1(l1, &Latch::latch, view.popupMenu_);
    Latch l2;
    xju::Observer<Latch> o2(l2, &Latch::latch, view.editTask_);
    
    std::cout << "double click task"
              << std::flush;
    ui.show();
    ui.run();
    xju::assert_equal(l1.value_, false);
    xju::assert_equal(l2.value_, true);
    l2.value_ = false;
    
    std::cout << "right-click task" << std::flush;
    ui.show();
    ui.run();
    xju::assert_equal(l1.value_, true);
    xju::assert_equal(l2.value_, false);
}

void test3(btt::Tasks& tasks,
	   btt::view::Tasks& view,
           UI& ui)
{
    tasks.insert(tasks.end(),
		 btt::Task(btt::TaskId(5),
			   "task 5 desc",
			   5*60*60,
			   boost::optional<xju::Time>()));
    
    std::cout << "verify list has 5, task 5 desc, 5.00 after task 1 (y for yes)"
	      << std::flush;
    ui.show();
    ui.run();
    
    xju::assert_equal(getResponse(), 'y');
}

void test4(btt::Tasks& tasks,
	   btt::view::Tasks& view,
           UI& ui)
{
    tasks.insert(boost::prior(tasks.end()),
		 btt::Task(btt::TaskId(7),
			   "task 7 desc",
			   77*60*60,
			   boost::optional<xju::Time>()));
    
    std::cout << "verify list has 7, task 7 desc, 77.00 after between 1 and 5 (y for yes)"
	      << std::flush;
    ui.show();
    ui.run();
    
    xju::assert_equal(getResponse(), 'y');
}

void test5(btt::Tasks& tasks,
	   btt::view::Tasks& view,
           UI& ui)
{
    tasks.insert(tasks.begin(),
		 btt::Task(btt::TaskId(9),
			   "task 9 desc",
			   79*60*60,
			   boost::optional<xju::Time>()));
    
    std::cout << "verify list has 9, 1, 7, 5 (y for yes)"
	      << std::flush;
    ui.show();
    ui.run();
    
    xju::assert_equal(getResponse(), 'y');
}

void test6(btt::Tasks& tasks,
	   btt::view::Tasks& view,
           UI& ui)
{
    tasks.insert(tasks.end(),
		 btt::Task(btt::TaskId(10),
			   "task 10 desc",
			   10*60*60,
			   boost::optional<xju::Time>()));
    
    tasks.erase(tasks.begin());
    
    std::cout << "verify list has 1, 7, 5, 10 (y for yes)"
	      << std::flush;
    ui.show();
    ui.run();
    
    xju::assert_equal(getResponse(), 'y');
}

void test7(btt::Tasks& tasks,
	   btt::view::Tasks& view,
           UI& ui)
{
    tasks.erase(boost::next(tasks.begin()));
    
    std::cout << "verify list has 1, 5, 10 (y for yes)"
	      << std::flush;
    ui.show();
    ui.run();
    
    xju::assert_equal(getResponse(), 'y');
}

void test8(btt::Tasks& tasks,
	   btt::view::Tasks& view,
           UI& ui)
{
    tasks.erase(boost::prior(tasks.end()));
    
    std::cout << "verify list has 1, 5 (y for yes)"
	      << std::flush;
    ui.show();
    ui.run();
    
    xju::assert_equal(getResponse(), 'y');
}

void test9(btt::Tasks& tasks,
	   btt::view::Tasks& view,
           UI& ui)
{
    tasks.erase(boost::prior(tasks.end()));
    
    std::cout << "verify list has 1 (y for yes)"
	      << std::flush;
    ui.show();
    ui.run();
    
    xju::assert_equal(getResponse(), 'y');
}

void test10(btt::Tasks& tasks,
	    btt::view::Tasks& view,
           UI& ui)
{
    tasks.erase(boost::prior(tasks.end()));
    
    std::cout << "verify list is empty (y for yes)"
	      << std::flush;
    ui.show();
    ui.run();
    
    xju::assert_equal(getResponse(), 'y');
}

int main(int argc, char* argv[])
{
    unsigned int n(0);
    
    btt::Tasks tasks;
    
    UI ui;
    btt::view::Tasks view(ui.o, tasks);
    
    test1(tasks, view, ui); ++n;
    test2(tasks, view, ui); ++n;
    test3(tasks, view, ui); ++n;
    test4(tasks, view, ui); ++n;
    test5(tasks, view, ui); ++n;
    test6(tasks, view, ui); ++n;
    test7(tasks, view, ui); ++n;
    test8(tasks, view, ui); ++n;
    test9(tasks, view, ui); ++n;
    test10(tasks, view, ui); ++n;

    std::cout << "Pass - " << n << " steps" << std::endl;
    return 0;
}
