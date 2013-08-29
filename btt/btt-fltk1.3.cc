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

#include <iostream>
#include <string>
#include <btt/Controller.hh>
#include <btt/fltk1.3-view/CurrentTask.hh>
#include <btt/fltk1.3-view/Tasks.hh>
#include <xju/Exception.hh>
#include <xju/Observer.hh>
#include <btt/fltk1.3-view/BttLayout.h>
#include <btt/fltk1.3-view/Callback.hh>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Menu.H>
#include <map>
#include <xju/assert.hh>
#include <iomanip>
#include <btt/workDone.hh>
#include <xju/stringToTime.hh>
#include <btt/RollUpReport.hh>

namespace timeSheetReport
{
    struct SummariseTask
    {
	SummariseTask(const btt::Tasks& tasks) throw():
	    tasks_(tasks)
	{
	}
	std::string operator()(
	    const std::map<btt::TaskId, unsigned int>::value_type& x) const
	    throw()
	{
	    const btt::Tasks::const_iterator t(
		std::find_if(tasks_.begin(),
			     tasks_.end(),
			     btt::Task::HasId(x.first)));
	    xju::assert_not_equal(t, tasks_.end());
	    
	    const double hours((double)x.second / 60.0 / 60.0);
	    
	    std::ostringstream s;
	    s.setf(std::ios_base::left);
	    s << std::setw(40) << (*t).description_.value()
	      << " "
	      << xju::format::float_(hours, std::ios_base::fixed, 1);
	    return s.str();
	}
    private:
	const btt::Tasks& tasks_;
    };
}

class TimeSheetReport
{
public:
    TimeSheetReport(const btt::WorkLog& workLog,
                    const btt::Tasks& tasks) throw();
    
    //
    // Report, in separate window, tasks worked on in last week (i.e.
    // week ending tonight at midnight), showing how many
    // hours was spent on each.
    // 
    void trigger() throw(xju::Exception);
    
    ~TimeSheetReport() throw() // avoid gcc warning
    {
    }
    
private:
    const btt::WorkLog& workLog_;
    const btt::Tasks& tasks_;
};

TimeSheetReport::TimeSheetReport(const btt::WorkLog& workLog,
                                 const btt::Tasks& tasks) throw():
    workLog_(workLog),
    tasks_(tasks)
{
}

void TimeSheetReport::trigger() throw(xju::Exception)
{
    try
    {
        const time_t now(::time(0));
        struct tm t;
        localtime_r(&now, &t);
        ++t.tm_mday;
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        
        const xju::Time endOfWeek(::mktime(&t), 0);
        const unsigned int secondsPerDay(24*60*60);
        std::ostringstream s;
        for(xju::Time startOfDay(endOfWeek.getSecs() -
                                 7U * secondsPerDay,
                                 0);
            startOfDay != endOfWeek;
            startOfDay = startOfDay + xju::MicroSeconds(secondsPerDay*1000000))
        {
            const xju::Time endOfDay(startOfDay + 
                                     xju::MicroSeconds(secondsPerDay*1000000));
            
            const std::map<btt::TaskId, unsigned int> x(
                btt::workDone(workLog_.rbegin(),
                              workLog_.rend(),
                              startOfDay,
                              endOfDay));
            s << xju::formatTime(startOfDay, "%a")
              << ":" << std::endl
              << xju::format::join(
                  x.begin(),
                  x.end(),
                  timeSheetReport::SummariseTask(tasks_),
                  "\n   ")
              << std::endl;
        }
//        Fl_Font*const oldFont(Fl::message_style->textfont());
//        Fl::message_style->textfont(Fl::COURIER);
//        Fl::message_style->textsize(Fl::message_style->textsize()+1);
        fl_message("%s", s.str().c_str());
    }
    catch(xju::Exception& e)
    {
        std::ostringstream s;
        s << "summarise work done in last week";
        e.addContext(s, XJU_TRACED);
        throw;
    }
}

class RollupTimeSheetReport
{
public:
    RollupTimeSheetReport(const btt::WorkLog& workLog,
                          const btt::Tasks& tasks) throw();
    
    //
    // Report, in separate window, tasks worked on in last week (i.e.
    // week ending tonight at midnight), showing how many
    // hours was spent on each, rolling up tasks according to
    // their description prefix (up to first ':').
    // 
    void trigger() throw(xju::Exception);
    
    ~RollupTimeSheetReport() throw() // avoid gcc warning
    {
    }
    
private:
    const btt::WorkLog& workLog_;
    const btt::Tasks& tasks_;
};

RollupTimeSheetReport::RollupTimeSheetReport(const btt::WorkLog& workLog,
                                             const btt::Tasks& tasks) throw():
    workLog_(workLog),
    tasks_(tasks)
{
}

void RollupTimeSheetReport::trigger() throw(xju::Exception)
{
    try
    {
        const time_t now(::time(0));
        struct tm t;
        localtime_r(&now, &t);
        ++t.tm_mday;
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        
        const xju::Time endOfWeek(::mktime(&t), 0);
        const uint64_t secondsPerDay(24*60*60);
        std::ostringstream s;
        for(xju::Time startOfDay(endOfWeek.getSecs() -
                                 7U * secondsPerDay,
                                 0);
            startOfDay != endOfWeek;
            startOfDay = startOfDay + xju::MicroSeconds(secondsPerDay*1000000))
        {
            const xju::Time endOfDay(startOfDay + 
                                     xju::MicroSeconds(secondsPerDay*1000000));
           
            std::map<std::string, xju::Seconds> totals;
            btt::RollUpReport r(tasks_, totals);
            
            const std::map<btt::TaskId, unsigned int> x(
                btt::workDone(workLog_.rbegin(),
                              workLog_.rend(),
                              startOfDay,
                              endOfDay));
            std::for_each(x.begin(), x.end(), r);
            
            s << xju::formatTime(startOfDay, "%a")
              << ":" << std::endl
              << r
              << std::endl;
        }
        // fltk::Font*const oldFont(fltk::message_style->textfont());
        // fltk::message_style->textfont(fltk::COURIER);
        // fltk::message_style->textsize(fltk::message_style->textsize()+1);
        fl_message("%s", s.str().c_str());
    }
    catch(xju::Exception& e)
    {
        std::ostringstream s;
        s << "summarise work done in last week";
        e.addContext(s, XJU_TRACED);
        throw;
    }
}

class Actions
{
public:
    Fl_Window& window_;
    btt::Controller& controller_;
    btt::Tasks::const_iterator end_;
    btt::Tasks::const_iterator const& selectedTask_;
    Fl_Menu& popupMenu_;
    Fl_Item& undo_;
    Fl_Item& redo_;
    Fl_Item& newTask_;
    Fl_Item& editSelected_;
    
    Actions(fltk::Window& window,
            btt::Controller& controller,
            btt::Tasks::const_iterator end,
            btt::Tasks::const_iterator const& selectedTask,
            fltk::PopupMenu& popupMenu,
            fltk::Item& undo,
            fltk::Item& redo,
            fltk::Item& newTask,
            fltk::Item& editSelected) throw():
        window_(window),
        controller_(controller),
        end_(end),
        selectedTask_(selectedTask),
        popupMenu_(popupMenu),
        undo_(undo),
        redo_(redo),
        newTask_(newTask),
        editSelected_(editSelected)
    {
        canUndoChanged();
        canRedoChanged();
        selectedTaskChanged();
    }
    void newTaskAtEnd() throw(xju::Exception)
    {
        controller_.addTask(end_);
    }
    void newTask() throw(xju::Exception)
    {
        controller_.addTask(selectedTask_);
    }
    void undo() throw(xju::Exception)
    {
        if (controller_.canUndo())
        {
            controller_.undo();
        }
    }
    void redo() throw(xju::Exception)
    {
        if (controller_.canRedo())
        {
            controller_.redo();
        }
    }
    void editSelected() throw(xju::Exception)
    {
        if (selectedTask_ != controller_.tasks().end())
        {
            const char* x = fltk::input(
                "Description:",
                (*selectedTask_).description_.value().c_str());
            if (x)
            {
                controller_.setDescription(selectedTask_, x);
            }
        }
    }
    void popupMenu() throw(xju::Exception)
    {
        if (selectedTask_ != controller_.tasks().end())
        {
            popupMenu_.value(-1);
            popupMenu_.popup(fltk::Rectangle(fltk::event_x(),
                                             fltk::event_y(),
                                             0,
                                             0));
        }
    }
    void canUndoChanged() throw(xju::Exception)
    {
        if (controller_.canUndo())
        {
            undo_.activate();
        }
        else
        {
            undo_.deactivate();
        }
    }
    void canRedoChanged() throw(xju::Exception)
    {
        if (controller_.canRedo())
        {
            redo_.activate();
        }
        else
        {
            redo_.deactivate();
        }
    }
    void selectedTaskChanged() throw(xju::Exception)
    {
        if (selectedTask_ == controller_.tasks().end())
        {
            newTask_.deactivate();
            editSelected_.deactivate();
        }
        else
        {
            newTask_.activate();
            editSelected_.activate();
        }
    }
    void startSelectedTaskAt() throw(xju::Exception)
    {
        if (selectedTask_ != controller_.tasks().end())
        {
            std::string prev;
            
            while(1)
            {
                try
                {
                    const char* x = fltk::input(
                        "%s", 
                        prev.c_str(),
                        "Working on task since, e.g. "
                        "20020425 9:30am (or just 9:30am)");
                    if (x)
                    {
                        prev=x;
                        const xju::Time t(xju::stringToTime(x));
                        controller_.recordWorkingOnTask(selectedTask_, t);
                    }
                    return;
                }
                catch(xju::Exception const& e)
                {
                    fltk::alert("%s", readableRepr(e).c_str());
                }
            }
        }
    }
    
};

class StartTask : public btt::view::Callback<StartTask>
{
public:
    btt::Controller& controller_;
    btt::Tasks::const_iterator const& task_;
    xju::MicroSeconds const offset_;
    
    StartTask(fltk::Item& menuItem,
              btt::Controller& controller,
              btt::Tasks::const_iterator const& task,
              xju::MicroSeconds offset) throw():
        btt::view::Callback<StartTask>(menuItem,
                                       *this,
                                       &StartTask::start),
        controller_(controller),
        task_(task),
        offset_(offset)
    {
    }
    void start() throw(xju::Exception)
    {
        const xju::Time x(xju::Time::now() - offset_);
        controller_.recordWorkingOnTask(task_, x);
    }
};


int main(int argc, char* argv[])
{
    if (argc < 4)
    {
	std::cout << "usage: btt <tasks-schema-file> <tasks-file> "
		  << "<work-log-file>" << std::endl;
	return 1;
    }
	
    const std::string tasksSchemaFile(argv[1]);
    const std::string tasksFile(argv[2]);
    const std::string workLogFile(argv[3]);
    
    try
    {
	btt::Controller controller(tasksFile,
				   tasksSchemaFile,
				   workLogFile);

        BttLayout ui;
	btt::view::CurrentTask currentTask(*ui.currentTask,
					   controller.workLog(),
					   controller.tasks());
	btt::view::Tasks tasks(*ui.tasks, controller.tasks());

        Actions actions(*ui.window,
                        controller,
                        controller.tasks().end(),
                        tasks.selectedTask_.value(),
                        *ui.startMenu,
                        *ui.undo,
                        *ui.redo,
                        *ui.newTask,
                        *ui.editSelected);
        btt::view::Callback<Actions> newTaskAction(
            *ui.newTask, actions, &Actions::newTask);
        btt::view::Callback<Actions> newTaskAtEndAction(
            *ui.newTaskAtEnd, actions, &Actions::newTaskAtEnd);

        btt::view::Callback<Actions> editSelectedAction(
            *ui.editSelected, actions, &Actions::editSelected);
        xju::Observer<btt::view::Callback<Actions> > editSelectedObserver(
            editSelectedAction,
            &btt::view::Callback<Actions>::trigger,
            tasks.editTask_);
        btt::view::Callback<Actions> undoAction(
            *ui.undo, actions, &Actions::undo);
        btt::view::Callback<Actions> redoAction(
            *ui.redo, actions, &Actions::redo);

        xju::Observer<Actions> popupMenu(
            actions,
            &Actions::popupMenu,
            tasks.popupMenu_);
        
        StartTask startNow(*ui.startNow,
                           controller,
                           tasks.selectedTask_.value(),
                           xju::MicroSeconds(0U));
        
        StartTask startMinus5(*ui.startMinus5,
                              controller,
                              tasks.selectedTask_.value(),
                              xju::MicroSeconds(5*60*1000000));
        
        StartTask startMinus15(*ui.startMinus15,
                               controller,
                               tasks.selectedTask_.value(),
                               xju::MicroSeconds(15*60*1000000));
        
        StartTask startMinus30(*ui.startMinus30,
                               controller,
                               tasks.selectedTask_.value(),
                               xju::MicroSeconds(30*60*1000000));
        
        StartTask startMinus45(*ui.startMinus45,
                               controller,
                               tasks.selectedTask_.value(),
                               xju::MicroSeconds(45*60*1000000UL));
        
        StartTask startMinus60(*ui.startMinus60,
                               controller,
                               tasks.selectedTask_.value(),
                               xju::MicroSeconds(60*60*1000000UL));
        
        StartTask startMinus90(*ui.startMinus90,
                               controller,
                               tasks.selectedTask_.value(),
                               xju::MicroSeconds(90*60*1000000UL));
        
        StartTask startMinus120(*ui.startMinus120,
                                controller,
                                tasks.selectedTask_.value(),
                                xju::MicroSeconds(120*60*1000000UL));

        btt::view::Callback<Actions> startAt(
            *ui.startAt, actions, &Actions::startSelectedTaskAt);
        
        TimeSheetReport timeSheetReport(controller.workLog(),
                                        controller.tasks());

        btt::view::Callback<TimeSheetReport> timeSheetReportAction(
            *ui.timeSheet, timeSheetReport, &TimeSheetReport::trigger);

        RollupTimeSheetReport rollupTimeSheetReport(controller.workLog(),
                                                    controller.tasks());

        btt::view::Callback<RollupTimeSheetReport> rollupTimeSheetReportAction(
            *ui.rollupTimeSheet, 
            rollupTimeSheetReport, 
            &RollupTimeSheetReport::trigger);

        xju::Observer<Actions> canUndo(actions,
                                       &Actions::canUndoChanged,
                                       controller.canUndoChanged());
        xju::Observer<Actions> canRedo(actions,
                                       &Actions::canRedoChanged,
                                       controller.canRedoChanged());
        
        xju::Observer<Actions> selectedTaskChanged(
            actions,
            &Actions::selectedTaskChanged,
            tasks.selectedTask_.m_changed);
        
        ui.window->show();
        fltk::run();
        
	return 0;
    }
    catch(const xju::Exception& e)
    {
	std::cerr << readableRepr(e) << std::endl;
	return 1;
    }
}
