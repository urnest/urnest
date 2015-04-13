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
#include <btt/Controller.hh>


#include <iostream>
#include <xju/assert.hh>
#include <btt/Tasks.hh>
#include <btt/XmlStore.hh>
#include <sstream>
#include <xju/formatTime.hh>
#include <xju/format.hh>
#include <xju/next.hh>


void test1(const std::string& schemaFileName,
	   const std::string& tasksFileName,
	   const std::string& workLogFileName)
{
    {
	btt::Tasks tasks;
        btt::XmlStore store;
	store.saveTasks(tasks, tasksFileName);
    }
    btt::Controller controller(tasksFileName, schemaFileName, workLogFileName);
    xju::assert_equal(controller.canUndo(), false);
    xju::assert_equal(controller.canRedo(), false);
    
    controller.addTask(controller.tasks().end());
    xju::assert_equal(controller.tasks().size(), 1U);
    
    const btt::Tasks::const_iterator i(controller.tasks().begin());

    xju::assert_equal((*i).id_, btt::TaskId(1));
    xju::assert_equal(controller.canUndo(), true);
    xju::assert_equal(controller.canRedo(), false);

    controller.undo();
    xju::assert_equal(controller.canUndo(), false);
    xju::assert_equal(controller.canRedo(), true);
    
    xju::assert_equal(controller.tasks().size(), 0U);

    controller.redo();
    xju::assert_equal(controller.canUndo(), true);
    xju::assert_equal(controller.canRedo(), false);
    xju::assert_equal(controller.tasks().size(), 1U);
    xju::assert_equal((*controller.tasks().begin()).id_, btt::TaskId(1));

    controller.undo();
    xju::assert_equal(controller.canUndo(), false);
    xju::assert_equal(controller.canRedo(), true);
    
    xju::assert_equal(controller.tasks().size(), 0U);
}


void test2(const std::string& schemaFileName,
	   const std::string& tasksFileName,
	   const std::string& workLogFileName)
{
    {
	btt::Tasks tasks;
	tasks.insert(
	    tasks.end(),
	    btt::Task(
		btt::TaskId(19),
		"des1",
		1,
		xju::Optional<xju::Time>()));
	tasks.insert(
	    tasks.end(),
	    btt::Task(
		btt::TaskId(1),
		"des2",
		2,
		xju::Optional<xju::Time>()));
	
        btt::XmlStore store;
	store.saveTasks(tasks, tasksFileName);
    }
    btt::Controller controller(tasksFileName, schemaFileName, workLogFileName);
    controller.addTask(controller.tasks().end());
    
    btt::Tasks::const_iterator i(controller.tasks().end());
    --i;

    xju::assert_equal((*i).id_, btt::TaskId(20));
    xju::assert_equal((*i).description_.value(), "");
    xju::assert_equal((*i).secondsSpentOnThisTask_.value(), 0U);
    xju::assert_equal(controller.tasks().size(), 3U);
}


// before, after previous
void test3(const std::string& schemaFileName,
	   const std::string& tasksFileName,
	   const std::string& workLogFileName)
{
    const xju::Time tt1(xju::Time::now());
    const xju::Time tt2(tt1 + xju::MicroSeconds(1000000));
    
    {
	btt::Tasks tasks;
	tasks.insert(
	    tasks.end(),
	    btt::Task(
		btt::TaskId(19),
		"des1",
		1,
		xju::Optional<xju::Time>()));
	tasks.insert(
	    tasks.end(),
	    btt::Task(
		btt::TaskId(1),
		"des2",
		2,
		xju::Optional<xju::Time>(xju::Time(5, 0))));
	
        btt::XmlStore store;
	store.saveTasks(tasks, tasksFileName);

	::unlink(workLogFileName.c_str());
	
	btt::WorkLog l(workLogFileName);
	l.logWorkingOnTaskSince(btt::TaskId(1), tt2);
    }
    btt::Controller controller(tasksFileName, schemaFileName, workLogFileName);
    xju::assert_equal(controller.tasks().size(), 2U);
    const btt::Task& t19(*controller.tasks().begin());
    const btt::Task& t1(*xju::next(controller.tasks().begin()));
    xju::assert_equal(t19.id(), btt::TaskId(19U));
    xju::assert_equal(t1.id(), btt::TaskId(1U));
    
    xju::assert_equal(controller.canUndo(), false);
    xju::assert_equal(controller.canRedo(), false);
    
    const std::string tt1s(xju::formatTime(tt1));
    const std::string tt2s(xju::formatTime(tt2));
    
    try
    {
	controller.recordWorkingOnTask(controller.tasks().begin(), tt1);
	abort();
    }
    catch(const xju::Exception& e)
    {
	xju::assert_equal(
	    readableRepr(e),
	    std::string(
		"Failed to record that you have been working on task with "
		"id 19 since ") + tt1s + std::string(" because\n"
		"the last entry in log file " + workLogFileName + 
		" indicates that you started "
		"working on task with id 1 at ") + tt2s + std::string(", "
		"which is after ") + tt1s + std::string(". Functionality "
		"to record this fact is not implemented."));
    }

    xju::assert_equal(controller.canUndo(), false);
    xju::assert_equal(controller.canRedo(), false);
    
    controller.recordWorkingOnTask(
	controller.tasks().begin(), tt2+xju::MicroSeconds(1000000));
    xju::assert_equal(
	t1.secondsSpentOnThisTask_.value(),
	3U);
    xju::assert_equal(t19.started_.value().valid(), true);
    xju::assert_equal(
	t19.secondsSpentOnThisTask_.value(),
	1U);

    xju::assert_equal(controller.canUndo(), true);
    xju::assert_equal(controller.canRedo(), false);

    controller.undo();
    xju::assert_equal(controller.canUndo(), false);
    xju::assert_equal(controller.canRedo(), true);
    
    xju::assert_equal(
	t1.secondsSpentOnThisTask_.value(),
	2U);
    xju::assert_equal(t19.started_.value().valid(), false);
    xju::assert_equal(
	t19.secondsSpentOnThisTask_.value(),
	1U);

    xju::assert_equal(controller.canUndo(), false);
    xju::assert_equal(controller.canRedo(), true);

    controller.redo();
    
    xju::assert_equal(
	t1.secondsSpentOnThisTask_.value(),
	3U);
    xju::assert_equal(t19.started_.value().valid(), true);
    xju::assert_equal(
	t19.secondsSpentOnThisTask_.value(),
	1U);

    xju::assert_equal(controller.canUndo(), true);
    xju::assert_equal(controller.canRedo(), false);

}


// no previous
void test4(const std::string& schemaFileName,
	   const std::string& tasksFileName,
	   const std::string& workLogFileName)
{
    {
	btt::Tasks tasks;
	tasks.insert(
	    tasks.end(),
	    btt::Task(
		btt::TaskId(1),
		"des2",
		0,
		xju::Optional<xju::Time>()));
	
        btt::XmlStore store;
	store.saveTasks(tasks, tasksFileName);

	::unlink(workLogFileName.c_str());
    }
    btt::Controller controller(tasksFileName, schemaFileName, workLogFileName);
    xju::assert_equal(controller.tasks().size(), 1U);
    const btt::Task& t1(*controller.tasks().begin());
    xju::assert_equal(t1.id(), btt::TaskId(1U));
    
    controller.recordWorkingOnTask(controller.tasks().begin(),
				   xju::Time(11, 0));
    xju::assert_not_equal(controller.workLog().rbegin(),
			  controller.workLog().rend());
    xju::assert_equal(t1.started_.value().valid(), true);
    xju::assert_equal(t1.secondsSpentOnThisTask_.value(), 0U);

    xju::assert_equal(controller.canUndo(), true);
    xju::assert_equal(controller.canRedo(), false);

    controller.undo();
    xju::assert_equal(controller.canUndo(), false);
    xju::assert_equal(controller.canRedo(), true);
    
    xju::assert_equal(controller.tasks().size(), 1U);
    xju::assert_equal(controller.workLog().rbegin(),
			  controller.workLog().rend());
    xju::assert_equal(t1.started_.value().valid(), false);
    xju::assert_equal(t1.secondsSpentOnThisTask_.value(), 0U);

    controller.redo();
    xju::assert_not_equal(controller.workLog().rbegin(),
			  controller.workLog().rend());
    xju::assert_equal(t1.started_.value().valid(), true);
    xju::assert_equal(t1.secondsSpentOnThisTask_.value(), 0U);

    xju::assert_equal(controller.canUndo(), true);
    xju::assert_equal(controller.canRedo(), false);
}

// setDescription
void test5(const std::string& schemaFileName,
	   const std::string& tasksFileName,
	   const std::string& workLogFileName)
{
    {
	btt::Tasks tasks;
	tasks.insert(
	    tasks.end(),
	    btt::Task(
		btt::TaskId(1),
		"des2",
		0,
		xju::Optional<xju::Time>()));
	
        btt::XmlStore store;
	store.saveTasks(tasks, tasksFileName);

	::unlink(workLogFileName.c_str());
    }
    {
	btt::Controller controller(tasksFileName,
				   schemaFileName,
				   workLogFileName);

	controller.setDescription(controller.tasks().begin(), "des2a");

        const btt::Task& t(*controller.tasks().begin());
	xju::assert_equal(t.description_.value(), "des2a");

        xju::assert_equal(controller.canUndo(), true);
        xju::assert_equal(controller.canRedo(), false);

        controller.undo();
        xju::assert_equal(controller.canUndo(), false);
        xju::assert_equal(controller.canRedo(), true);
        
	xju::assert_equal(t.description_.value(), "des2");

        controller.redo();
	xju::assert_equal(t.description_.value(), "des2a");

        xju::assert_equal(controller.canUndo(), true);
        xju::assert_equal(controller.canRedo(), false);
    }
    {
	btt::Controller controller(tasksFileName,
				   schemaFileName,
				   workLogFileName);
        const btt::Task& t(*controller.tasks().begin());
	xju::assert_equal(t.description_.value(), "des2a");

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
    if (argc != 2)
    {
	std::cerr << "pass tasks.xsd full path name as only parameter"
		  << std::endl;
	return 1;
    }
    
    TempFile f1;
    TempFile f2;
    
    unsigned int n(0);
    
    test1(argv[1], f1.name(), f2.name()); ++n;
    test2(argv[1], f1.name(), f2.name()); ++n;
    test3(argv[1], f1.name(), f2.name()); ++n;
    test4(argv[1], f1.name(), f2.name()); ++n;
    test5(argv[1], f1.name(), f2.name()); ++n;
    // REVISIT: verify addTask saves tasks

    std::cout << "Pass - " << n << " steps" << std::endl;
    return 0;
}
