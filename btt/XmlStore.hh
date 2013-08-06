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
//    Loading/saving of Tasks to xml format file.
//
#ifndef _BTT_XMLSTORE_HH_
#define _BTT_XMLSTORE_HH_


#include <btt/Tasks.hh>
#include <string>
#include <xju/Exception.hh>
#include <utility>


namespace btt
{
    class XmlStore
    {
    public:
	//
	// Load Tasks from the specified file, validating it
	// against the specified schema.
	//
	// post: file' = file
	//
	std::auto_ptr<Tasks> loadTasks(
	    const std::string& fileName,
	    const std::string& tasksSchemaFileName) throw(
		//
		// Failed to validate tasks against schema, or
		// Other failure.
		//
		// post: file' = file
		//
		xju::Exception);
	
	//
	// Save specified Tasks to the specified file, using
	// file.new as a temporary store.
	//
	// Operation is atomic in that file is guarenteed to either
	// contain its prior content or the specified tasks even
	// in the event of O/S crash (see rename(2)).
	//
	void saveTasks(const Tasks& tasks, const std::string fileName) throw(
	    //
	    // post: file' = file
	    //       file.new might exist
	    //
	    xju::Exception);
    };
}


#endif
