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
#include <xju/Time.hh>


#include <sys/time.h>
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <xju/assert.hh>
#include <functional>
#include <sys/types.h>


namespace xju
{
    Time::Time(unsigned long secs, unsigned int usecs) throw():
        _secs(secs),
        _usecs(usecs)
    {
        xju::assert_(usecs, std::less<unsigned int>(), 1000000U);
    }
    
    bool Time::operator==(const Time &b) const throw()
    {
        return (_secs == b._secs && _usecs == b._usecs);
    }
    
    bool Time::operator<(const Time &b) const throw()
    {
        if (_secs > b._secs)
            return false;
        
        if (_secs < b._secs)
            return true;
        
        if (_usecs < b._usecs)
            return true;
        
        return false;
    }
    bool Time::operator>(const Time& x) const throw()
    {
        return !operator<(x) && !operator==(x); 
    }
    bool Time::operator<=(const Time& x) const throw()
    {
        return operator<(x) || operator==(x);
    }
    bool Time::operator>=(const Time& x) const throw()
    {
        return !operator<(x);
    }
    
    
    unsigned long Time::getSecs() const throw()
    {
        return _secs;
    }
    
    unsigned int Time::getUsecs() const throw()
    {
        return _usecs;
    }
    
    Time Time::now() throw()
    {
#ifndef __MINGW32__
        timeval now;
        gettimeofday(&now, 0);
        
        //
        // timeval.tv_sec is typically signed, but we're
        // converting it to unsigned
        //
        xju::assert_(now.tv_sec, std::greater_equal<time_t>(), 0);
        
        return Time(now.tv_sec, now.tv_usec);
#else
        time_t now;
        time(&now);
// REVISIT get usec from somewhere
        return Time(now, 0);
#endif
    }
    
}
xju::Time operator+(const xju::Time &t1, const xju::MicroSeconds &t2) throw()
{
    unsigned int usecs = t1.getUsecs() + t2.value()%1000000;
    unsigned long secs = t1.getSecs() + t2.value()/1000000 + (usecs / 1000000U);
    usecs = usecs % 1000000U;
    return xju::Time(secs, usecs);
}

xju::Time operator-(xju::Time const& t, 
                    xju::MicroSeconds const& d) throw()
{
    xju::MicroSeconds const a(t - xju::Time(0, 0));
    xju::MicroSeconds const b(a - d);
    return xju::Time(b.value() / 1000000, b.value() % 1000000);
}

xju::MicroSeconds operator-(const xju::Time &t1, const xju::Time &t2) throw()
{
    xju::assert_(t1, std::greater_equal<xju::Time>(), t2);
    unsigned long secs = 0;
    unsigned int usecs = 0;
    
    if (t1.getUsecs() >= t2.getUsecs()) {
        usecs = t1.getUsecs() - t2.getUsecs();
        secs = t1.getSecs() - t2.getSecs();
    } else { // t1.getUsecs() < t2.getUsecs()
        usecs = (1000000U - t2.getUsecs()) + t1.getUsecs();
        secs = (t1.getSecs()-t2.getSecs()-1UL);
    }
    return xju::MicroSeconds(((uint64_t)secs)*1000000+usecs);
}

std::ostream& operator<<(std::ostream& s, const xju::Time& x) throw()
{
    s << x.getSecs() << "." << xju::format::int_(x.getUsecs(), 6);
    return s;
}
