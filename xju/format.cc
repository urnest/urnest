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
#include <xju/format.hh>
#include <sstream>
#include <xju/isnan.hh>
#include <ctype.h>
#include <iomanip>

namespace xju
{
    namespace format
    {
	std::string char_(const char c) throw()
	{
	    std::ostringstream s;
	    s << int_(c);
	    if (isalnum(c))
	    {
		s << "('" << c << "')";
	    }
	    return s.str();
	}

	std::string int_(long x, 
			 int width, 
			 char fill,
			 ios_base::fmtflags align,
			 ios_base::fmtflags base) throw()
	{
	    std::ostringstream s;
	    s.setf(align, ios_base::adjustfield);
	    s.fill(fill);
	    s.width(width);
	    s.setf(base, ios_base::basefield);
	    s << x;
	    return s.str();
	}
	
	std::string int_(unsigned long x, 
			 int width, 
			 char fill,
			 ios_base::fmtflags align,
			 ios_base::fmtflags base) throw()
	{
	    std::ostringstream s;
	    s.setf(align, ios_base::adjustfield);
	    s.fill(fill);
	    s.width(width);
	    s.setf(base, ios_base::basefield);
	    s << x;
	    return s.str();
	}
	
	std::string float_(const float x, 
			   const ios_base::fmtflags format,
			   const int precision) throw()
	{
	    if (xju_isnan(x))
	    {
		return "nan";
	    }
	    std::ostringstream s;
	    s.setf(format, std::ios::floatfield);
	    s.precision(precision);
	    s << x;
	    return s.str();
	}
	
	std::string float_(const double x, 
			   const ios_base::fmtflags format,
			   const int precision) throw()
	{
	    if (xju_isnan(x))
	    {
		return "nan";
	    }
	    std::ostringstream s;
	    s.setf(format, std::ios::floatfield);
	    s.precision(precision);
	    s << x;
	    return s.str();
	}
	
	std::string quote(const std::string& x) throw()
	{
	    return quote("\"", x);
	}
	
	std::string quote(const std::string& quote,
			  const std::string& x) throw()
	{
	    return quote + x + quote;
	}
	
	std::string quote(const std::string& pre,
			  const std::string& post,
			  const std::string& x) throw()
	{
	    return pre + x + post;
	}
        std::string hex(char x, const std::string& leader) 
            throw(std::bad_alloc)
        {
            std::ostringstream s;
            s << leader
              << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') << (long)x;
            return s.str();
        }
        
        std::string hex(unsigned char x, const std::string& leader) 
            throw(std::bad_alloc)
        {
            std::ostringstream s;
            s << leader
              << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') << (unsigned long)x;
            return s.str();
        }
        std::string hex(short x, const std::string& leader) 
            throw(std::bad_alloc)
        {
            std::ostringstream s;
            s << leader
              << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') << (long)x;
            return s.str();
        }
        std::string hex(unsigned short x, const std::string& leader) 
            throw(std::bad_alloc)
        {
            std::ostringstream s;
            s << leader
              << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') << (unsigned long)x;
            return s.str();
        }
        std::string hex(int x, const std::string& leader) 
            throw(std::bad_alloc)
        {
            std::ostringstream s;
            s << leader
              << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') << (long)x;
            return s.str();
        }
        std::string hex(unsigned int x, const std::string& leader) 
            throw(std::bad_alloc)
        {
            std::ostringstream s;
            s << leader
              << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') << (unsigned long)x;
            return s.str();
        }
        std::string hex(long x, const std::string& leader) 
            throw(std::bad_alloc)
        {
            std::ostringstream s;
            s << leader
              << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') << (long)x;
            return s.str();
        }
        std::string hex(unsigned long x, const std::string& leader) 
            throw(std::bad_alloc)
        {
            std::ostringstream s;
            s << leader
              << std::hex << std::setw(sizeof(x)*2) << std::setfill('0') << (unsigned long)x;
            return s.str();
        }
    }
}
