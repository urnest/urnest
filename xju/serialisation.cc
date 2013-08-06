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
//    implements serialisation.hh
//
#include <xju/serialisation.hh>

namespace xju
{
    namespace serialisation
    {
        namespace
        {
            bool byteSwapped_() throw()
            {
                const unsigned short x(1);
                return *(const char*)&x == 0;
            }
        }
        const bool byteSwapped(byteSwapped_());
    }
}

