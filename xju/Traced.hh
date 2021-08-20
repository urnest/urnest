// Copyright (c) 2007 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    Utilities to capture file and line number information.
//
#ifndef _XJU_TRACED_HH_
#define _XJU_TRACED_HH_


#include <string>
#include <utility>


//
// Short-hand way to construct a xju::Traced that captures the
// current (at-point-of-use) file and line number.
//
#define XJU_TRACED xju::Traced(std::string(__FILE__), __LINE__)


namespace xju
{
    class Traced : public std::pair<std::string, unsigned int>
    {
    public:
	//
	// Store specified file and line number.
	//
	// post: file() = file
	//       line() = line
	//
	Traced(std::string file,
               unsigned int line) throw():
            std::pair<std::string, unsigned int>(std::move(file),
                                                 std::move(line))
	{
	}
        explicit Traced(std::pair<std::string,unsigned int> x) throw():
            std::pair<std::string, unsigned int>(std::move(x))
	{
	}
        
	const std::string& file() const throw()
	{
	    return first;
	}
	unsigned int line() const throw()
	{
	    return second;
	}

    private:		
        friend bool operator<(Traced const& x, Traced const& y) throw()
        {
            return std::make_pair(x.file(), x.line()) <
                std::make_pair(y.file(), y.line());
        }
        friend bool operator==(Traced const& x, Traced const& y) throw()
        {
            return !(x<y||y<x);
        }
        friend bool operator!=(Traced const& x, Traced const& y) throw()
        {
            return !(x==y);
        }
        friend bool operator>(Traced const& x, Traced const& y) throw()
        {
            return y<x;
        }
        friend bool operator>=(Traced const& x, Traced const& y) throw()
        {
            return x>y || x==y;
        }
        friend bool operator<=(Traced const& x, Traced const& y) throw()
        {
            return x<y || x==y;
        }
    };
}


//
// In the unlikely event of a macro name clash with XJU_TRACED
// here is an alternative that is less likely to clash.
//
#define XJU_TRACED_NOCLASH_7Q6 ::xju::Traced(std::string(__FILE__), __LINE__)


#endif
