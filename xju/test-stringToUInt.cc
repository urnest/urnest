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
#include <xju/stringToUInt.hh>


#include <iostream>
#include <stdlib.h>
#include <string>
#include <xju/assert.hh>
#include <sstream>
#include <limits.h>

// good
void test1()
{
    xju::assert_equal(xju::stringToUInt("234"), 234U);
}

// bad character
void test2()
{
    try
    {
	xju::stringToUInt("2B4");
	abort();
    }
    catch(const xju::Exception& e)
    {
	std::ostringstream s;
	s << e;
	xju::assert_equal(
	    s.str(),
	    "Failed to convert \"2B4\" to a base-10 unsigned integer because\n"
	    "character 2 ('B') of \"2B4\" unexpected.");
    }
}


// overflow - don't compile unless sizeof(long) > sizeof(int)
struct Test3
{
    template<int X>
    void t()
    {
        try
        {
            std::ostringstream s;
            const unsigned long x(UINT_MAX);
            s << (x+1L);
            
            xju::stringToUInt(s.str());
            abort();
        }
        catch(const xju::Exception& e)
        {
            std::ostringstream s;
            s << e;
            xju::assert_equal(
                s.str(),
                "Failed to convert \"4294967296\" to a base-10 unsigned integer because\n4294967296 is too large (maximum allowed unsigned integer is 4294967295).");
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
