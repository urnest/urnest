// generated by Fast Light User Interface Designer (fluid) version 1.0302

#include "BttLayout.h"

Fl_Menu_Item BttLayout::menu_menubar[] = {
 {"Task", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"&New Task before Selected Task", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"New Task at &End", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"&Properties...", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Edit", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"&Undo", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"&Redo", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Report", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"Time Sheet", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"Rollup Timesheet", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {"Start", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"Now", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};
Fl_Menu_Item* BttLayout::taskMenu = BttLayout::menu_menubar + 0;
Fl_Menu_Item* BttLayout::newTask = BttLayout::menu_menubar + 1;
Fl_Menu_Item* BttLayout::newTaskAtEnd = BttLayout::menu_menubar + 2;
Fl_Menu_Item* BttLayout::editSelected = BttLayout::menu_menubar + 3;
Fl_Menu_Item* BttLayout::editMenu = BttLayout::menu_menubar + 5;
Fl_Menu_Item* BttLayout::undo = BttLayout::menu_menubar + 6;
Fl_Menu_Item* BttLayout::redo = BttLayout::menu_menubar + 7;
Fl_Menu_Item* BttLayout::reportMenu = BttLayout::menu_menubar + 9;
Fl_Menu_Item* BttLayout::timeSheet = BttLayout::menu_menubar + 10;
Fl_Menu_Item* BttLayout::rollupTimeSheet = BttLayout::menu_menubar + 11;
Fl_Menu_Item* BttLayout::startMenu = BttLayout::menu_menubar + 13;
Fl_Menu_Item* BttLayout::startNow = BttLayout::menu_menubar + 14;

BttLayout::BttLayout() {
  { window = new Fl_Window(395, 180);
    window->user_data((void*)(this));
    { menubar = new Fl_Menu_Bar(0, 0, 395, 20);
      menubar->menu(menu_menubar);
    } // Fl_Menu_Bar* menubar
    { currentTask = new Fl_Output(0, 20, 395, 25);
    } // Fl_Output* currentTask
    { tasks = new Fl_Browser(0, 45, 393, 136);
    } // Fl_Browser* tasks
    window->end();
  } // Fl_Window* window
}