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
//    Controlled modification to work log file and tasks.
//
#ifndef _BTT_CONTROLLER_HH_
#define _BTT_CONTROLLER_HH_


#include <string>
#include <xju/Exception.hh>
#include <xju/Event.hh>
#include <btt/Tasks.hh>
#include <btt/WorkLog.hh>
#include <xju/Notifying.hh>
#include <btt/XmlStore.hh>
#include <memory>
#include <xju/Shared.hh>


namespace btt
{
    class Controller : xju::NonCopyable
    {
    public:
	Controller(const std::string& tasksFileName,
		   const std::string& tasksSchemaFileName,
		   const std::string& workLogFileName) throw(
		       xju::Exception);
	
	//
	// Add a new task to tasks after the specified task, assigning
	// the next TaskId unused within tasks.
	//
	// post: canUndo()
	//       !canRedo()
	//
	void addTask(Tasks::const_iterator before) throw(
	    xju::Exception);


	//
	// Record that the user has been working on the specified
	// task since the specified time.
	//
	// pre:   tasks.begin() <= task < tasks().end()
	// post:  (*workLog.rend()).second' = *task
	//        (*workLog.rend()).first' = since
	//        (*task).started_.get() ||
	//           (*(*task).started_.value()) = since 
	//        no previous active task ||
	//           previous active task's secondsSpentOnThisTask updated
	//
	// post: canUndo()
	//       !canRedo()
	//
	void recordWorkingOnTask(
	    const Tasks::const_iterator task,
	    const xju::Time& since) throw(
		//
		// pre: start would precede previous tasks' start, or
		//      other failure.
		// post: no change
		//
		xju::Exception);
	

	//
	// Modify the description of the specified task.
	//
	// post: canUndo()
	//       !canRedo()
	//
	void setDescription(
	    const Tasks::const_iterator task,
	    const std::string& description) throw(xju::Exception);
	
	const Tasks& tasks() const throw();
	const WorkLog& workLog() const throw();

	bool canUndo() const throw();
	bool canRedo() const throw();

	const xju::Event& canUndoChanged() const throw();
	const xju::Event& canRedoChanged() const throw();
	
	//
	// pre: canUndo()
	// post: canRedo()
	//
	void undo() throw(
	    // post: *this = *this@pre
	    xju::Exception);

	//
	// pre: canRedo()
	// post: canUndo()
	//
	void redo() throw(
	    // post: *this = *this@pre
	    xju::Exception);
	
    private:
	const std::string tasksFileName_;
	XmlStore tasksStore_;
        std::auto_ptr<Tasks> tasks_;
	WorkLog workLog_;

	void saveTasks() throw(xju::Exception);

    public:
	class Cmd;
	class Undo
	{
	public:
	    virtual ~Undo() throw()
	    {
	    }
	    virtual std::auto_ptr<Cmd> operator()() throw(xju::Exception) = 0;
	};
	class Cmd
	{
	public:
	    virtual ~Cmd() throw()
	    {
	    }
	    virtual std::auto_ptr<Undo> operator()() throw(xju::Exception) = 0;
	};
    private:
	typedef std::vector<xju::Shared<Undo> > UndoList;
	typedef std::vector<xju::Shared<Cmd> > RedoList;

	//
	// Undo and redo lists. Last item is the first to
	// undo/redo.
	//
	xju::Notifying<UndoList> undoList_;
	xju::Notifying<RedoList> redoList_;

	//
	// Adjust our undo/redo lists assuming we just
	// redid/undid the specified item (after removing it
	// from the redo/undo list.
	//
	void redid(xju::Shared<Undo> x) throw();
	void undid(xju::Shared<Cmd> x) throw();

        //
        // Perform the specified command.
        //
        // - saves changes
        // - registers undo item for command (via did)
        //
        void doCmd(std::auto_ptr<Cmd> x) throw(xju::Exception);
        
	//
	// Adjust our undo/redo lists assuming we just
	// did the specified (new) operation:
	//
	// - clear redo list
	// - append x to undo list
	// - trim undo list if it is too long
	//
	void did(std::auto_ptr<Undo> x) throw();
    };
}


#endif
