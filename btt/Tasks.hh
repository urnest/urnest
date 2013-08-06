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
//    Tasks.
//
#ifndef _BTT_TASKS_HH_
#define _BTT_TASKS_HH_


#include <xju/NotifyingList.hh>
#include <btt/Task.hh>


namespace btt
{
    typedef xju::NotifyingList<Task> Tasks;
}


#endif
