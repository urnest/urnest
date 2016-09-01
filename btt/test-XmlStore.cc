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
#include <btt/XmlStore.hh>

#include <utility>
#include <xju/assert.hh>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <fstream>
#include <unistd.h>
#include <xju/format.hh>
#include <xju/unistd.hh>

using namespace btt;


// can't open tasks file
void test1(const std::string& schemaFileName,
	   const std::string)
{
    XmlStore store;

    try
    {
	store.loadTasks("no-such-file.xml", schemaFileName);
    }
    catch(xju::Exception& e)
    {
        // REVISIT: hard-coded directory
	xju::assert_equal(
	    readableRepr(e),
            "Failed to read tasks.xml file no-such-file.xml (current directory is " + xju::getcwd_() + ") because\nfailed to read contents of file no-such-file.xml because\nopen system call failed, errno = 2.");
    }
}


// no tasks
void test2(const std::string& schemaFileName,
	   const std::string& f)
{
    Tasks tasks;
    
    XmlStore store;
    
    store.saveTasks(tasks, f);
    
    std::unique_ptr<Tasks> x(store.loadTasks(f, schemaFileName));

    xju::assert_equal(x->size(), 0U);
}


// 1 task
void test3(const std::string& schemaFileName,
	   const std::string& f)
{
    Tasks tasks;
    tasks.insert(
	tasks.end(),
	Task(TaskId(1),
	     "td!@#$%^&*()_-+=[]{}|\\\"':;<,>.?/~`",
	     32,
	     xju::Optional<xju::Time>()));
    
    XmlStore store;
    
    store.saveTasks(tasks, f);
    
    std::unique_ptr<Tasks> x(store.loadTasks(f, schemaFileName));

    xju::assert_equal(x->size(), 1U);
    xju::assert_equal((*x->begin()).id_, TaskId(1U));
    xju::assert_equal((*x->begin()).description_.value(), 
                      std::string("td!@#$%^&*()_-+=[]{}|\\\"':;<,>.?/~`"));
    xju::assert_equal((*x->begin()).secondsSpentOnThisTask_.value(), 32U);
    xju::assert_equal((*x->begin()).started_.value().valid(), false);
}


// can't open schema file
void test4(const std::string& schemaFileName,
	   const std::string& badTasks1,
	   const std::string)
{
    XmlStore store;

    try
    {
	store.loadTasks(badTasks1, "/dev/null/tasks.xsd");
    }
    catch(xju::Exception& e)
    {
#ifdef __MINGW32__
	xju::assert_equal(
	    readableRepr(e),
	    "Failed to load tasks from file " + badTasks1 +
	    " because\n"
	    "failed to verify tasks schema file /dev/null/tasks.xsd is "
	    "readable because\n"
	    "open system call failed, errno = 2.");
#else
	xju::assert_equal(
	    readableRepr(e), std::string("Failed to read tasks.xml file " + badTasks1 + " (current directory is " + xju::getcwd_() +") because\nfailed to parse tasks at line 2 column 1 because\nexpected Expanded Name \"http://v1.timetracker.hfmpmobiles.australia.boeing.com:tasks\" tag, got Expanded Name \"timetracker.hfmpmobiles.australia.boeing.com:taskss\"."));
#endif
    }
}


// bad content in tasks file
void test5(const std::string& schemaFileName,
	   const std::string& badTasks1,
	   const std::string)
{
    XmlStore store;

    try
    {
	store.loadTasks(badTasks1,
			schemaFileName);
    }
    catch(xju::Exception& e)
    {
	xju::assert_equal(e.context().size(), 2U);
	xju::assert_equal(
	    e.context()[1].first,
	    "read tasks.xml file " + badTasks1 + " (current directory is " +
            xju::getcwd_() + ")");
	xju::assert_equal(
	    e.context()[0].first,
	    "parse tasks at line 2 column 1");
    }
}

// 3 tasks
void test6(const std::string& schemaFileName,
	   const std::string& f)
{
    Tasks tasks;
    tasks.insert(
	tasks.end(),
	Task(TaskId(1),
	     "td",
	     32,
	     xju::Optional<xju::Time>(xju::Time(1, 0))));
    tasks.insert(
	tasks.end(),
	Task(TaskId(3),
	     "td 3",
	     33,
	     xju::Optional<xju::Time>()));
    tasks.insert(
	tasks.end(),
	Task(TaskId(5),
	     "td \"5\"",
	     35,
	     xju::Optional<xju::Time>()));
    
    XmlStore store;
    
    store.saveTasks(tasks, f);
    
    std::unique_ptr<Tasks> x(store.loadTasks(f, schemaFileName));

    xju::assert_equal(x->size(), 3U);

    Tasks::const_iterator i(x->begin());
    
    xju::assert_equal((*i).id_, TaskId(1U));
    xju::assert_equal((*i).description_.value(), "td");
    xju::assert_equal((*i).secondsSpentOnThisTask_.value(), 32U);
    xju::assert_equal((*i).started_.value().valid(), true);
    xju::assert_equal(*(*i).started_.value(), xju::Time(1, 0));
    
    ++i;
    xju::assert_equal((*i).id_, TaskId(3U));
    xju::assert_equal((*i).description_.value(), "td 3");
    xju::assert_equal((*i).secondsSpentOnThisTask_.value(), 33U);
    xju::assert_equal((*i).started_.value().valid(), false);

    ++i;
    xju::assert_equal((*i).id_, TaskId(5U));
    xju::assert_equal((*i).description_.value(), "td \"5\"");
    xju::assert_equal((*i).secondsSpentOnThisTask_.value(), 35U);
    xju::assert_equal((*i).started_.value().valid(), false);
}


void test7(const std::string& schemaFileName,
	   const std::string& f)
{
    {
	std::ofstream s(f.c_str());
	s << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>" << std::endl
	  << "<btt:tasks xmlns:btt=\"http://v1.timetracker.hfmpmobiles.australia.boeing.com\">" << std::endl
	  << "<task id=\"1\" description=\"Home\"/>" << std::endl
	  << "</btt:tasks>" << std::endl;
	xju::assert_not_equal(s.good(), 0);
    }

    XmlStore store;

    try
    {
	store.loadTasks(f, schemaFileName);
	abort();
    }
    catch(const xju::Exception& e)
    {
	xju::assert_equal(e.context().size(), 2U);
	xju::assert_equal(
	    e.context()[0].first,
	    std::string("parse tasks at line 2 column 1"));
	xju::assert_equal(
	    e.context()[1].first,
	    std::string("read tasks.xml file " + f +
                        " (current directory is " +
                        xju::getcwd_() + ")"));
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
	name_(mkName(next_)),
        release_(false)
    {
    }
    std::string name() const throw()
    {
	return name_;
    }
    void release() throw()
    {
        release_ = true;
    }
    
    ~TempFile() throw()
    {
        if (release_)
        {
            ::unlink(name_.c_str());
        }
        else
        {
            std::cout << "left " << name_ << " behind" << std::endl;
        }
    }
    
private:
    const std::string name_;
    bool release_;
    
    static int next_;

    static const std::string mkName(int& next) throw()
    {
	return std::string("tmp_")+xju::format::int_(next++);
    }
};
int TempFile::next_(1);


int main(int argc, char* argv[])
{
    if (argc != 3)
    {
	std::cerr << "pass tasks.xsd full path name as 1st parameter, "
		  << "bad-tasks-1.xml full path name as 2nd parameter"
		  << std::endl;
	return 1;
    }

    unsigned int n(0);
    
    try
    {
	TempFile f1;
	
	test1(argv[1], f1.name()); ++n;
	test2(argv[1], f1.name()); ++n;
	test3(argv[1], f1.name()); ++n;
	test4(argv[1], argv[2], f1.name()); ++n;
	test5(argv[1], argv[2], f1.name()); ++n;
	test6(argv[1], f1.name()); ++n;
	test7(argv[1], f1.name()); ++n;
	f1.release();
        
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
