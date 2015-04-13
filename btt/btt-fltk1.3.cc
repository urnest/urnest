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

#include <FL/Fl_Hold_Browser.H>
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
    Fl_Menu_Item& undo_;
    Fl_Menu_Item& redo_;
    Fl_Menu_Item& newTask_;
    Fl_Menu_Item& editSelected_;
    
    Actions(Fl_Window& window,
            btt::Controller& controller,
            btt::Tasks::const_iterator end,
            btt::Tasks::const_iterator const& selectedTask,
            Fl_Menu_Item& undo,
            Fl_Menu_Item& redo,
            Fl_Menu_Item& newTask,
            Fl_Menu_Item& editSelected) throw():
        window_(window),
        controller_(controller),
        end_(end),
        selectedTask_(selectedTask),
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
            const char* x = fl_input(
                "Description:",
                (*selectedTask_).description_.value().c_str());
            if (x)
            {
                controller_.setDescription(selectedTask_, x);
            }
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
                    const char* x = fl_input(
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
                    fl_alert("%s", readableRepr(e).c_str());
                }
            }
        }
    }
    
};

class Starter
{
public:
    Starter(Fl_Menu_Item& menuItem,
            btt::Controller& controller,
            btt::Tasks::const_iterator const& selectedTask,
            xju::MicroSeconds const& ago) throw():
        controller_(controller),
        selectedTask_(selectedTask),
        ago_(ago),
        callback_(menuItem, *this, &Starter::start) {
    }
    btt::Controller& controller_;
    btt::Tasks::const_iterator const& selectedTask_;
    xju::MicroSeconds const ago_;
    
    btt::view::Callback<Starter, Fl_Menu_Item> const callback_;
    
    void start() throw(xju::Exception)
    {
        if (selectedTask_ != controller_.tasks().end())
        {
            const xju::Time x(xju::Time::now()-ago_);
            controller_.recordWorkingOnTask(selectedTask_, x);
        }
    }
};

class TasksCallback
{
public:
    explicit TasksCallback(
        Fl_Menu_Item& startMenu,
        Actions& actions,
        Fl_Menu_Item& startNowItem, Starter& startNow,
        Fl_Menu_Item& startMinus5Item, Starter& startMinus5,
        Fl_Menu_Item& startMinus10Item, Starter& startMinus10,
        Fl_Menu_Item& startMinus15Item, Starter& startMinus15,
        Fl_Menu_Item& startMinus30Item, Starter& startMinus30,
        Fl_Menu_Item& startMinus45Item, Starter& startMinus45,
        Fl_Menu_Item& startMinus60Item, Starter& startMinus60,
        Fl_Menu_Item& startMinus90Item, Starter& startMinus90,
        Fl_Menu_Item& startMinus120Item, Starter& startMinus120,
        Fl_Menu_Item& startAt) throw():
        startMenu_(startMenu),
        actions_(actions),
        startNowItem_(startNowItem),
        startNow_(startNow),
        startMinus5Item_(startMinus5Item),
        startMinus5_(startMinus5),
        startMinus10Item_(startMinus10Item),
        startMinus10_(startMinus10),
        startMinus15Item_(startMinus15Item),
        startMinus15_(startMinus15),
        startMinus30Item_(startMinus30Item),
        startMinus30_(startMinus30),
        startMinus45Item_(startMinus45Item),
        startMinus45_(startMinus45),
        startMinus60Item_(startMinus60Item),
        startMinus60_(startMinus60),
        startMinus90Item_(startMinus90Item),
        startMinus90_(startMinus90),
        startMinus120Item_(startMinus120Item),
        startMinus120_(startMinus120),
        startAt_(startAt)
    {
    }
    Fl_Menu_Item& startMenu_;

    Fl_Menu_Item& startNowItem_;
    Starter& startNow_;
    Fl_Menu_Item& startMinus5Item_;
    Starter& startMinus5_;
    Fl_Menu_Item& startMinus10Item_;
    Starter& startMinus10_;
    Fl_Menu_Item& startMinus15Item_;
    Starter& startMinus15_;
    Fl_Menu_Item& startMinus30Item_;
    Starter& startMinus30_;
    Fl_Menu_Item& startMinus45Item_;
    Starter& startMinus45_;
    Fl_Menu_Item& startMinus60Item_;
    Starter& startMinus60_;
    Fl_Menu_Item& startMinus90Item_;
    Starter& startMinus90_;
    Fl_Menu_Item& startMinus120Item_;
    Starter& startMinus120_;
    Fl_Menu_Item& startAt_;

    Actions& actions_;
    
    void trigger() throw()
    {
        if (Fl::event_clicks()) {
            std::cout << "tasks double click" << std::endl;
            actions_.editSelected();
        }
        else if (Fl::event()==FL_RELEASE &&
                 Fl::event_button()==FL_RIGHT_MOUSE) {
            std::cout << "tasks right click" << std::endl;
            Fl_Menu_Item const* m(
                startMenu_.popup(Fl::event_x(), Fl::event_y()));
            if (m==&startNowItem_) {
                startNow_.start();
            }
            else if (m==&startMinus5Item_) {
                startMinus5_.start();
            }
            else if (m==&startMinus10Item_) {
                startMinus10_.start();
            }
            else if (m==&startMinus15Item_) {
                startMinus15_.start();
            }
            else if (m==&startMinus30Item_) {
                startMinus30_.start();
            }
            else if (m==&startMinus45Item_) {
                startMinus45_.start();
            }
            else if (m==&startMinus60Item_) {
                startMinus60_.start();
            }
            else if (m==&startMinus90Item_) {
                startMinus90_.start();
            }
            else if (m==&startMinus120Item_) {
                startMinus120_.start();
            }
            else if (m==&startAt_) {
                actions_.startSelectedTaskAt();
            }
        }
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
                        *ui.undo,
                        *ui.redo,
                        *ui.newTask,
                        *ui.editSelected);
        btt::view::Callback<Actions, Fl_Menu_Item> newTaskAction(
            *ui.newTask, actions, &Actions::newTask);
        btt::view::Callback<Actions, Fl_Menu_Item> newTaskAtEndAction(
            *ui.newTaskAtEnd, actions, &Actions::newTaskAtEnd);

        btt::view::Callback<Actions, Fl_Menu_Item> editSelectedAction(
            *ui.editSelected, actions, &Actions::editSelected);
        btt::view::Callback<Actions, Fl_Menu_Item> undoAction(
            *ui.undo, actions, &Actions::undo);
        btt::view::Callback<Actions, Fl_Menu_Item> redoAction(
            *ui.redo, actions, &Actions::redo);

        Starter startNow(*ui.startNow,
                         controller,
                         tasks.selectedTask_.value(),
                         xju::MicroSeconds(0U));

        Starter startMinus5(*ui.start5,
                            controller,
                            tasks.selectedTask_.value(),
                            xju::MicroSeconds(5*60*1000000));
        
        Starter startMinus10(*ui.start10,
                             controller,
                             tasks.selectedTask_.value(),
                             xju::MicroSeconds(5*60*1000000));
        
        Starter startMinus15(*ui.start15,
                             controller,
                             tasks.selectedTask_.value(),
                             xju::MicroSeconds(15*60*1000000));
        
        Starter startMinus30(*ui.start30,
                             controller,
                             tasks.selectedTask_.value(),
                             xju::MicroSeconds(30*60*1000000));
        
        Starter startMinus45(*ui.start45,
                             controller,
                             tasks.selectedTask_.value(),
                             xju::MicroSeconds(45*60*1000000UL));
        
        Starter startMinus60(*ui.start60,
                             controller,
                             tasks.selectedTask_.value(),
                             xju::MicroSeconds(60*60*1000000UL));
        
        Starter startMinus90(*ui.start90,
                             controller,
                             tasks.selectedTask_.value(),
                             xju::MicroSeconds(90*60*1000000UL));
        
        Starter startMinus120(*ui.start120,
                              controller,
                              tasks.selectedTask_.value(),
                              xju::MicroSeconds(120*60*1000000UL));
        
        btt::view::Callback<Actions, Fl_Menu_Item> startAt(
            *ui.startAt, actions, &Actions::startSelectedTaskAt);
        
        TasksCallback tcb(*ui.startNow, actions,
                          *ui.startNow, startNow,
                          *ui.start5, startMinus5,
                          *ui.start10, startMinus10,
                          *ui.start15, startMinus15,
                          *ui.start30, startMinus30,
                          *ui.start45, startMinus45,
                          *ui.start60, startMinus60,
                          *ui.start90, startMinus90,
                          *ui.start120, startMinus120,
                          *ui.startAt);
        xju::Observer<TasksCallback> tcbl(
            tcb, &TasksCallback::trigger, tasks.callback_);
        
        TimeSheetReport timeSheetReport(controller.workLog(),
                                        controller.tasks());

        btt::view::Callback<TimeSheetReport, Fl_Menu_Item>
            timeSheetReportAction(
                *ui.timeSheet, timeSheetReport, &TimeSheetReport::trigger);

        RollupTimeSheetReport rollupTimeSheetReport(controller.workLog(),
                                                    controller.tasks());

        btt::view::Callback<RollupTimeSheetReport, Fl_Menu_Item>
            rollupTimeSheetReportAction(
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
        Fl::run();
        
	return 0;
    }
    catch(const xju::Exception& e)
    {
	std::cerr << readableRepr(e) << std::endl;
	return 1;
    }
}
