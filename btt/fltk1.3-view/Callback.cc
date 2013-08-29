//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 2007
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include "btt/fltk-view/Callback.hh"

#include <fltk/ask.h>

namespace btt
{
    namespace view
    {
        void Callback_handle(xju::Exception const& e) throw()
        {
            fltk::alert("%s", readableRepr(e).c_str());
        }
    }
}

        
