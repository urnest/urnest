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


#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <xju/assert.hh>


// good
void test1()
{
	xju::assert_equal(xju::stringToDouble("234.5"), 234.5);
}


// bad character
void test2()
{
	try
	{
		xju::stringToDouble("234.5A");
		abort();
	}
	catch(const xju::Exception& e)
	{
		std::ostringstream s;
		s << e;
		xju::assert_equal(
			s.str(),
			"Failed to convert \"234.5A\" to a double because\n"
			"character 6 ('A') of 234.5A unexpected.");
	}
}


int main(int argc, char* argv[])
{
	unsigned int n(0);
	try
	{
		test1(); ++n;
		test2(); ++n;
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
