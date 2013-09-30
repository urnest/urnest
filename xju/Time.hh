//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 2007 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    Wrapper for gettimeofday(2).
//
//    Rationale:
//
//       - basic comparisons, difference etc allow use for timeouts
//       - no "formatting" or "parsing" as it is too contentious (inividual
//         users - e.g. log file timestamp class - are in much better
//         position to provide appropraite formatting and parsing).
//
#ifndef _XJU_TIME_HH_
#define _XJU_TIME_HH_

#include <string>
#include "xju/MicroSeconds.hh"

namespace xju
{
    class Time {
    public:
	//
	// pre: usecs < 1000000
	//
	Time(unsigned long secs, unsigned int usecs) throw();
	
	bool operator<(const Time &) const throw();
	bool operator==(const Time &) const throw();
	bool operator>(const Time& x) const throw();
	bool operator<=(const Time& x) const throw();
	bool operator>=(const Time& x) const throw();
	bool operator!=(const Time& x) const throw()
	{
	    return !(operator==(x));
	}
	
	unsigned long getSecs() const throw();
	unsigned int getUsecs() const throw(); // result < 1000000
	
	static Time now() throw(); // since epoch (see gettimeofday(2))
	
    private:
	unsigned long _secs;
	unsigned int _usecs;
    };
}

xju::Time operator+(xju::Time const& t, 
                    xju::MicroSeconds const& d) throw(); // REVISIT: overflow?

// pre: t >= d
xju::Time operator-(xju::Time const& t, 
                    xju::MicroSeconds const& d) throw();

// pre: t1 >= t2
xju::MicroSeconds operator-(const xju::Time &t1, const xju::Time &t2) throw();

//
// format is seconds.microseconds. Result has 6 digits
// after the decimal and no leading zeros, e.g. 2445234.324070
//
std::ostream& operator<<(std::ostream& s, const xju::Time& x) throw();


#endif
