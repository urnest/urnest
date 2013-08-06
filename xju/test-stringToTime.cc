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
#include <xju/stringToTime.hh>


#include <iostream>
#include <time.h>
#include <xju/assert.hh>
#include <xju/format.hh>
#include <stdlib.h>
#include <sstream>


namespace
{
#ifdef __MINGW32__
    struct tm* localtime_r(const time_t* t, struct tm* result)
    {
        // REVISIT: need serialisation
        *result = *::localtime(t);
        return result;
    }
#endif
}

void test1()
{
    struct tm x;
    x.tm_year = 2002 - 1900;
    x.tm_mon = 3;
    x.tm_mday = 25;
    x.tm_hour = 8;
    x.tm_min = 0;
    x.tm_sec = 0;
    x.tm_yday = 0;
    x.tm_isdst = 0;
    
    time_t y(mktime(&x));
    struct tm z;
    localtime_r(&y, &z);
    xju::assert_equal(x.tm_year, z.tm_year);
    xju::assert_equal(x.tm_mon, z.tm_mon);
    xju::assert_equal(x.tm_mday, z.tm_mday);
    xju::assert_equal(x.tm_hour, z.tm_hour);
    xju::assert_equal(x.tm_min, z.tm_min);
    xju::assert_equal(x.tm_sec, z.tm_sec);
    
    xju::assert_equal(xju::stringToTime("20020425 8:00am"),
		      xju::Time(y, 0));
}

void test2()
{
    
    struct tm x;
    x.tm_year = 2002 - 1900;
    x.tm_mon = 3;
    x.tm_mday = 25;
    x.tm_hour = 0;
    x.tm_min = 0;
    x.tm_sec = 0;
    time_t y(mktime(&x));
    
    xju::assert_equal(xju::stringToTime("20020425 12:00am"),
		      xju::Time(y, 0));
}

void test3()
{
    struct tm x;
    x.tm_year = 2002 - 1900;
    x.tm_mon = 3;
    x.tm_mday = 25;
    x.tm_hour = 12;
    x.tm_min = 0;
    x.tm_sec = 0;
    time_t y(mktime(&x));
    
    xju::assert_equal(xju::stringToTime("20020425 12:00pm"),
		      xju::Time(y, 0));
}

void test4()
{
    struct tm x;
    x.tm_year = 2002 - 1900;
    x.tm_mon = 3;
    x.tm_mday = 25;
    x.tm_hour = 9;
    x.tm_min = 23;
    x.tm_sec = 0;
    time_t y(mktime(&x));
    
    xju::assert_equal(xju::stringToTime("20020425 9:23am"),
		      xju::Time(y, 0));
}

void test5()
{
    try
    {
	xju::stringToTime("");
	abort();
    }
    catch(const xju::Exception& e)
    {
	std::ostringstream s;
	s << e;
	xju::assert_equal(
	    s.str(),
	    "Failed to parse \"\" as a time because\n\"\" does not look like "
	    "YYYYMMDD HH:MMam or HH:MMam.");
    }
}
void test6()
{
    struct tm x;
    time_t now(time(0));
    localtime_r(&now, &x);
    std::ostringstream s;
    s << xju::format::int_(x.tm_year+1900, 4)
      << xju::format::int_(x.tm_mon+1, 2)
      << xju::format::int_(x.tm_mday, 2)
      << " ";
    std::string s1(s.str()+" 12:30pm");
    xju::assert_equal(xju::stringToTime(s1),
                      xju::stringToTime("12:30pm"));
    std::string s2(s.str()+" 9:30am");
    xju::assert_equal(xju::stringToTime(s2),
                      xju::stringToTime("9:30am"));
}

int main(int argc, char* argv[])
{
    unsigned int n(0);
    
    test1(); ++n;
    test2(); ++n;
    test3(); ++n;
    test4(); ++n;
    test5(); ++n;
    test6(); ++n;
    
    std::cout << "PASS " << n << " steps" << std::endl;
    return 0;
}
