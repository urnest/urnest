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
//    REVISIT: use swap for exception safety (need xju::Notifier::swap)
//
#include <btt/Controller.hh>


#include <set>
#include <xju/Time.hh>
#include <xju/formatTime.hh>
#include <xju/next.hh>
#include <algorithm>
#include <xju/assert.hh>
#include <sstream>
#include <algorithm>

namespace btt
{
    namespace
    {
	TaskId getNextUnusedTaskId(const Tasks& tasks) throw()
	{
	    if (tasks.size() == 0)
	    {
		return TaskId(1);
	    }
	    std::set<TaskId> ids;
	    Tasks::const_iterator i;
	    for(i = tasks.begin(); i != tasks.end(); ++i)
	    {
		ids.insert((*i).id_);
	    }
	    return (*ids.rbegin()).next();
	}
	
	class UndoAddTask : public Controller::Undo
	{
	public:
	    ~UndoAddTask() throw()
	    {
	    }
	    UndoAddTask(Tasks& tasks,
			Tasks::const_iterator task) throw():
		tasks_(tasks),
		task_(task)
	    {
	    }
	    std::auto_ptr<Controller::Cmd> operator()() throw(xju::Exception);
	private:
	    Tasks& tasks_;
	public:
	    const Tasks::const_iterator task_;
	};
	class AddTask : public Controller::Cmd
	{
	public:
	    ~AddTask() throw()
	    {
	    }
	    AddTask(Tasks& tasks,
		    const Task& task,
		    Tasks::const_iterator successor) throw():
		tasks_(tasks),
		task_(task),
		successor_(successor)
	    {
	    }
	    std::auto_ptr<Controller::Undo> operator()() throw(xju::Exception);
	private:
	    Tasks& tasks_;
	    const Task task_;
	    const Tasks::const_iterator successor_;
	};
	
	std::auto_ptr<Controller::Cmd> UndoAddTask::operator()() throw(
	    xju::Exception)
	{
	    try
	    {
		std::auto_ptr<Controller::Cmd> result(
		    new AddTask(tasks_, *task_, xju::next(task_)));
		tasks_.erase(task_);
		return result;
	    }
	    catch(xju::Exception& e)
	    {
		e.addContext("undo add-task", XJU_TRACED);
		throw;
	    }
	}

	std::auto_ptr<Controller::Undo> AddTask::operator()() throw(
	    xju::Exception)
	{
	    try
	    {
		std::auto_ptr<Controller::Undo> result(
		    new UndoAddTask(tasks_, tasks_.insert(successor_, task_)));
		return result;
	    }
	    catch(xju::Exception& e)
	    {
                std::ostringstream s;
                s << "add a new task before task with id "
                  << (*successor_).id_;
                e.addContext(s, XJU_TRACED);
                throw;
	    }
	}

	class RecordWorkingOnTask;
	class UndoRecordWorkingOnTask : public Controller::Undo
	{
	public:
	    ~UndoRecordWorkingOnTask() throw()
	    {
	    }
	    UndoRecordWorkingOnTask(
		Tasks& tasks,
		WorkLog& workLog,
		xju::Optional<xju::Time> oldStarted) throw():
		tasks_(tasks),
		workLog_(workLog),
		oldStarted_(oldStarted)
	    {
	    }
	    std::auto_ptr<Controller::Cmd> operator()() throw(
		xju::Exception);
	private:
	    Tasks& tasks_;
	    WorkLog& workLog_;
	    const xju::Optional<xju::Time> oldStarted_;
	};
	class RecordWorkingOnTask : public Controller::Cmd
	{
	public:
	    ~RecordWorkingOnTask() throw()
	    {
	    }
	    RecordWorkingOnTask(
		Tasks& tasks,
		WorkLog& workLog,
		Tasks::const_iterator task,
		const xju::Time& since) throw():
		tasks_(tasks),
		workLog_(workLog),
		task_(task),
		since_(since)
	    {
	    }
	    
	    std::auto_ptr<Controller::Undo> operator()() throw(
		xju::Exception);
	private:
	    Tasks& tasks_;
	    WorkLog& workLog_;
	    const Tasks::const_iterator task_;
	    const xju::Time since_;
	};
	std::auto_ptr<Controller::Cmd>
	UndoRecordWorkingOnTask::operator()() throw(
	    xju::Exception)
	{
            try
            {
                const TaskId currentId((*workLog_.rbegin()).second);
                const Tasks::iterator current(
                    std::find_if(
                        tasks_.begin(),
                        tasks_.end(),
                        Task::HasId(currentId)));
                
                xju::assert_not_equal(current, tasks_.end());
                
                const xju::Time since((*workLog_.rbegin()).first);
                
                std::auto_ptr<Controller::Cmd> result(
                    new RecordWorkingOnTask(
                        tasks_,
                        workLog_,
                        current,
                        since));
                
                (*current).started_ = oldStarted_;
                workLog_.pop_back();
                if (workLog_.rbegin() != workLog_.rend())
                {
                    const TaskId prevId((*workLog_.rbegin()).second);
                    const Tasks::iterator prev(
                        std::find_if(
                            tasks_.begin(),
                            tasks_.end(),
                            Task::HasId(prevId)));
                    const xju::Time s((*workLog_.rbegin()).first);
                    const xju::MicroSeconds d(since - s);
                    (*prev).secondsSpentOnThisTask_ =
                        (*prev).secondsSpentOnThisTask_.value() - 
                        d.value()/1000000U;
                }
                return result;
            }
            catch(xju::Exception& e)
            {
                e.addContext("undo record-working-on-since", XJU_TRACED);
                throw;
            }
	}

	std::auto_ptr<Controller::Undo>
	RecordWorkingOnTask::operator()() throw(
	    xju::Exception)
	{
            try
            {
                const Tasks::iterator task(tasks_.mute(task_));
                
                if (workLog_.rbegin() != workLog_.rend())
                {
                    if (since_ < (*workLog_.rbegin()).first)
                    {
                        std::ostringstream r;
                        r << "the last entry in log file " 
                          << workLog_.fileName_
                          << " indicates that you started working on task "
                          << "with id "
                          << (*workLog_.rbegin()).second
                          << " at "
                          << xju::formatTime((*workLog_.rbegin()).first)
                          << ", which is after "
                          << xju::formatTime(since_)
                          << ". Functionality to record this fact is not "
                          << "implemented";
                        throw xju::Exception(r, XJU_TRACED);
                    }
                    
                    const TaskId prevId((*workLog_.rbegin()).second);
                    const Tasks::iterator prev(
                        std::find_if(
                            tasks_.begin(),
                            tasks_.end(),
                            Task::HasId(prevId)));
                    
                    xju::assert_not_equal(prev, tasks_.end());
		
                    const xju::MicroSeconds spent(since_ - 
                                              (*workLog_.rbegin()).first);
                    (*prev).secondsSpentOnThisTask_ =
                        (*prev).secondsSpentOnThisTask_.value() +
                        spent.value()/1000000;
                }
                
                workLog_.logWorkingOnTaskSince((*task).id_, since_);
                
                const xju::Optional<xju::Time> oldStarted(
                    (*task).started_.value());
                xju::Optional<xju::Time> xxx((*task).started_.value());
                if (!xxx.valid())
                {
                    (*task).started_ = xju::Optional<xju::Time>(since_);
                }
                
                std::auto_ptr<Controller::Undo> u(new UndoRecordWorkingOnTask(
                    tasks_,
                    workLog_,
                    oldStarted));
                return u;
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "record that you have been working on task with id "
                  << (*task_).id_
                  << " since "
                  << xju::formatTime(since_);
                e.addContext(s, XJU_TRACED);
                throw;
            }
	}

        class UndoSetDescription;
        class SetDescription : public Controller::Cmd
        {
        public:
            ~SetDescription() throw()
            {
            }
            SetDescription(Tasks& tasks,
                           const Tasks::const_iterator task,
                           const std::string& description) throw():
                tasks_(tasks),
                task_(task),
                description_(description)
            {
            }
            std::auto_ptr<Controller::Undo> operator()() throw(xju::Exception);
        private:
            Tasks& tasks_;
            const Tasks::const_iterator task_;
            const std::string description_;
        };
        class UndoSetDescription : public Controller::Undo
        {
        public:
            ~UndoSetDescription() throw()
            {
            }
            UndoSetDescription(Tasks& tasks,
                               const Tasks::const_iterator task,
                               const std::string& oldDescription) throw():
                tasks_(tasks),
                task_(task),
                oldDescription_(oldDescription)
            {
            }
            std::auto_ptr<Controller::Cmd> operator()() throw();
        private:
            Tasks& tasks_;
            const Tasks::const_iterator task_;
            const std::string oldDescription_;
        };
        
        std::auto_ptr<Controller::Undo> SetDescription::operator()() throw(
            xju::Exception)
        {
            try
            {
                std::auto_ptr<Controller::Undo> result(
                    new UndoSetDescription(tasks_,
                                           task_,
                                           (*task_).description_));
                const Tasks::iterator t(tasks_.mute(task_));
                (*t).description_ = description_;
                return result;
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "change task "
                  << (*task_).id_
                  << "'s description to "
                  << description_;
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }
        std::auto_ptr<Controller::Cmd> UndoSetDescription::operator()() throw()
        {
            try
            {
                std::auto_ptr<Controller::Cmd> result(
                    new SetDescription(tasks_,
                                       task_,
                                       (*task_).description_));
                const Tasks::iterator t(tasks_.mute(task_));
                (*t).description_ = oldDescription_;
                return result;
            }
            catch(xju::Exception& e)
            {
                e.addContext("undo set-description", XJU_TRACED);
                throw;
            }
        }
    }
    
    Controller::Controller(
	const std::string& tasksFileName,
	const std::string& tasksSchemaFileName,
	const std::string& workLogFileName) throw(
	    xju::Exception):
	tasksFileName_(tasksFileName),
	tasks_(tasksStore_.loadTasks(tasksFileName, tasksSchemaFileName)),
	workLog_(workLogFileName)
    {
    }

    void Controller::addTask(
	const Tasks::const_iterator before) throw(
	    xju::Exception)
    {
        const TaskId id(getNextUnusedTaskId(*tasks_));
        xju::Optional<xju::Time> started;
        std::auto_ptr<Cmd> d(new AddTask(
            *tasks_,
            btt::Task(id, "", 0, started),
            before));
        doCmd(d);
    }

    void Controller::recordWorkingOnTask(
	const Tasks::const_iterator ctask,
	const xju::Time& since) throw(
	    xju::Exception)
    {
        std::auto_ptr<Cmd> d(new RecordWorkingOnTask(
            *tasks_,
            workLog_,
            ctask,
            since));
        doCmd(d);
    }

    void Controller::setDescription(
	const Tasks::const_iterator task,
	const std::string& description) throw(xju::Exception)
    {
        std::auto_ptr<Controller::Cmd> d(
            new SetDescription(*tasks_, task, description));
        doCmd(d);
    }
    
    
    const Tasks& Controller::tasks() const throw()
    {
	return *tasks_;
    }
    
    const WorkLog& Controller::workLog() const throw()
    {
	return workLog_;
    }

    bool Controller::canUndo() const throw()
    {
	return undoList_.value().size() != 0;
    }
    
    bool Controller::canRedo() const throw()
    {
	return redoList_.value().size() != 0;
    }
    
    const xju::Event& Controller::canUndoChanged() const throw()
    {
	return undoList_.m_changed;
    }
    
    const xju::Event& Controller::canRedoChanged() const throw()
    {
	return redoList_.m_changed;
    }
    
    void Controller::undo() throw(xju::Exception)
    {
	try
	{
	    xju::Shared<Undo> x(undoList_.value().back());
	    xju::Shared<Cmd> y((*x)());
	    saveTasks();
	    undid(y);
	    
	    UndoList l(undoList_.value());
	    l.pop_back();
	    undoList_ = l;
	}
	catch(xju::Exception& e)
	{
	    std::ostringstream s;
	    s << "undo operation";
	    e.addContext(s, XJU_TRACED);
	    throw;
	}
    }

    void Controller::redo() throw(xju::Exception)
    {
	try
	{
	    xju::Shared<Cmd> x(redoList_.value().back());
	    xju::Shared<Undo> y((*x)());
	    saveTasks();
	    redid(y);
	    
	    RedoList l(redoList_.value());
	    l.pop_back();
	    redoList_ = l;
	}
	catch(xju::Exception& e)
	{
	    std::ostringstream s;
	    s << "redo operation";
	    e.addContext(s, XJU_TRACED);
	    throw;
	}
    }

    void Controller::saveTasks() throw(
	xju::Exception)
    {
	tasksStore_.saveTasks(*tasks_, tasksFileName_);
    }

    void Controller::redid(xju::Shared<Undo> x) throw()
    {
	UndoList l(undoList_.value());
	l.push_back(x);
	undoList_ = l;
    }

    void Controller::undid(xju::Shared<Cmd> x) throw()
    {
	RedoList l(redoList_.value());
	l.push_back(x);
	redoList_ = l;
    }

    void Controller::doCmd(std::auto_ptr<Cmd> d) throw(
        xju::Exception)
    {
        std::auto_ptr<Undo> u((*d)());
        
        try
        {
            saveTasks();
        }
        catch(xju::Exception& e)
        {
            try
            {
                (*u)();
            }
            catch(const xju::Exception& e)
            {
                abort();
            }
            throw;
        }
        did(u);
    }
    
    void Controller::did(std::auto_ptr<Undo> x) throw()
    {
	redoList_ = RedoList();
	UndoList l;
	if (undoList_.value().size() < 1000)
	{
	    l = undoList_;
	}
	else
	{
	    std::copy(xju::next(undoList_.value().begin()),
		      undoList_.value().end(),
		      std::back_inserter(l));
	}
	l.push_back(x);
	undoList_ = l;
    }
}
