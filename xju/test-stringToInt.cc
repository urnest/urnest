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
#include <xju/stringToInt.hh>


#include <iostream>
#include <stdlib.h>
#include <string>
#include <xju/assert.hh>
#include <sstream>
#include <limits.h>

// good
void test1()
{
    xju::assert_equal(xju::stringToInt("234"), 234);
}


// bad character
void test2()
{
    try
    {
	xju::stringToInt("2B4");
	abort();
    }
    catch(const xju::Exception& e)
    {
	std::ostringstream s;
	s << e;
	xju::assert_equal(
	    s.str(),
	    "Failed to convert \"2B4\" to an integer because\n"
	    "character 2 ('B') of \"2B4\" unexpected.");
    }
}


struct Test3
{
    template<int X>
    void t()
    {
        try
        {
            std::ostringstream s;
            const long x(INT_MAX);
            s << (x+1L);
            
            xju::stringToInt(s.str());
            abort();
        }
        catch(const xju::Exception& e)
        {
            std::ostringstream s;
            s << e;
            xju::assert_equal(
                 s.str(),
                 "Failed to convert \"2147483648\" to an integer because\n2147483648 is too large (maximum allowed integer is 2147483647).");
        }
    }
};

template<>
void Test3::t<0>()
{
}

// overflow - can't do unless sizeof(long) > sizeof(int)
void test3()
{
    // avoid compiler warning by not instatiating code unless
    // sizeof(long) > sizeof(int)
    Test3 t3;
    t3.t<sizeof(long)-sizeof(int)>();
}


int main(int argc, char* argv[])
{
    unsigned int n(0);
    try
    {
	test1(); ++n;
	test2(); ++n;
	test3(); ++n;
	std::cout << "PASS - " << n << " steps" << std::endl;
	return 0;
    }
    catch(xju::Exception& e)
    {
	std::ostringstream s;
	s << "do step " << (n+1);
	e.addContext(s, XJU_TRACED);
	
	std::cout << e << std::endl;
	return 1;
    }
}
