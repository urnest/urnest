//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 1997-2007 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/stringToDouble.hh>


#include <xju/format.hh>
#include <sstream>

namespace xju
{
    double stringToDouble(const std::string& value) throw(xju::Exception)
    {
        try
        {
            if (value.size() == 0)
            {
                std::ostringstream cause;
                cause << xju::format::quote(value) << " is null";
                throw xju::Exception(cause, XJU_TRACED);
            }
            std::size_t* p(0);
            const long double result(std::stod(value.c_str(), &p));
            if (p != value.size())
            {
                std::ostringstream cause;
                cause << "character " << (p + 1)
                      << " ('" << (value[p]) << "') of "
                      << value 
                      << " unexpected";
                throw xju::Exception(cause, XJU_TRACED);
            }
            return result;
        }
        catch(xju::Exception& e)
        {
            std::ostringstream s;
            s << "convert " << xju::format::quote(value) << " to a double";
            e.addContext(s, XJU_TRACED);
            throw;
        }
    }
}
