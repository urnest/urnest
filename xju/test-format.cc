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


#include <xju/assert.hh>
#include <iostream>
#include <vector>
#include <functional>
#include <set>
#include <math.h>


// int format
void test1()
{
    {
	std::string result(xju::format::int_(10));
	xju::assert_equal(result, std::string("10"));
    }
    {
	std::string result(xju::format::int_(20, 4));
	xju::assert_equal(result, std::string("0020"));
    }
    {
	std::string result(xju::format::int_(20, 6, ' ', std::ios::left));
	xju::assert_equal(result, std::string("20    "));
    }
}


// quote
void test2()
{
    {
	std::string result(xju::format::quote(std::string("x")));
	xju::assert_equal(result, std::string("\"x\""));
    }
    {
	std::string result(xju::format::quote("'", std::string("x")));
	xju::assert_equal(result, std::string("'x'"));
    }
    {
	std::string result(xju::format::quote("(", ")", std::string("x")));
	xju::assert_equal(result, std::string("(x)"));
    }
}

// join
void test3()
{
    {
	std::vector<int> x;
	std::string result = xju::format::join(x.begin(),
					       x.end(),
					       xju::format::Int(),
					       "-");
	xju::assert_equal(result, std::string(""));
    }
    {
	std::vector<int> x;
	x.push_back(1);
	
	std::string result = xju::format::join(x.begin(),
					       x.end(),
					       xju::format::Int(),
					       "-");
	xju::assert_equal(result, std::string("1"));
    }
    {
	std::vector<int> x;
	x.push_back(1);
	x.push_back(2);
	x.push_back(3);
	
	std::string result = xju::format::join(x.begin(),
					       x.end(),
					       xju::format::Int(2),
					       ", ");
	xju::assert_equal(result, std::string("01, 02, 03"));
    }
}


// set
void test4()
{
    // empty
    {
	std::set<int> x;
	std::string result = xju::format::set(x.begin(),
					      x.end(),
					      xju::format::Int());
	xju::assert_equal(result, std::string("{  }"));
    }
    // 1 element
    {
	std::set<int> x;
	x.insert(5);
	
	std::string result = xju::format::set(x.begin(),
					      x.end(),
					      xju::format::Int());
	xju::assert_equal(result, std::string("{ 5 }"));
    }
    // > 1 element
    {
	std::set<int> x;
	x.insert(5);
	x.insert(8);
	x.insert(10);
	
	std::string result = xju::format::set(x.begin(),
					      x.end(),
					      xju::format::Int());
	xju::assert_equal(result, std::string("{ 5, 8, 10 }"));
    }
}

class Y
{
public:
    Y(const std::string& name) throw():
	_name(name)
    {
    }
    const std::string _name;
};
std::ostream& operator<<(std::ostream& s, const Y& y) throw()
{
    s << "Y called " << y._name;
    return s;
}


// str()
void test5()
{
    xju::assert_equal(xju::format::str('a'), std::string("a"));
    xju::assert_equal(xju::format::str(true), std::string("true"));
    xju::assert_equal(xju::format::str(false), std::string("false"));
    xju::assert_equal(xju::format::str(126), std::string("126"));
    xju::assert_equal(xju::format::str(1268L), std::string("1268"));
    xju::assert_equal(xju::format::str(126U), std::string("126"));
    xju::assert_equal(xju::format::str(1268UL), std::string("1268"));
    xju::assert_equal(xju::format::str(0.5), std::string("0.5"));
    xju::assert_equal(xju::format::str(std::string("abcd")),
		      std::string("abcd"));
    xju::assert_equal(xju::format::str(Y("abcd")),
		      std::string("Y called abcd"));
}

// float_()
void test6()
{
    {
	xju::assert_equal(xju::format::Float()(12.34), "12.34");
	xju::assert_equal(xju::format::Float(
	    xju::ios_base::fmtflags(0), 3)(12.34), "12.3");
	xju::assert_equal(xju::format::Float(xju::ios_base::scientific)(12.34),
#ifdef __MINGW32__
			  "1.234000e+001"
#else
			  "1.234000e+01"
#endif
            );
	xju::assert_equal(xju::format::Float(xju::ios_base::fixed, 4)(12.34),
			  "12.3400");
    }
    {
	xju::assert_equal(xju::format::Double()(12.34), "12.34");
	xju::assert_equal(xju::format::Double(
	    xju::ios_base::fmtflags(0), 3)(12.34), "12.3");
	xju::assert_equal(
            xju::format::Double(xju::ios_base::scientific)(12.34),
#ifdef __MINGW32__
            "1.234000e+001"
#else
            "1.234000e+01"
#endif
            );
	xju::assert_equal(xju::format::Double(
	    xju::ios_base::fixed, 4)(12.34), "12.3400");
    }
    xju::assert_equal(xju::format::float_(sqrt(-1.0)), "nan");
}

void test7()
{
    // REVISIT: assume 32 bit system for now

    xju::assert_equal(xju::format::hex((char)0x3), "0x03");
    xju::assert_equal(xju::format::hex((char)0x43), "0x43");

    xju::assert_equal(xju::format::hex((short)0x3), "0x0003");
    xju::assert_equal(xju::format::hex((short)0x43), "0x0043");
    xju::assert_equal(xju::format::hex((short)0x143), "0x0143");
    xju::assert_equal(xju::format::hex((short)0x2143), "0x2143");

    xju::assert_equal(xju::format::hex((int)0x3), "0x00000003");
    xju::assert_equal(xju::format::hex((int)0x43), "0x00000043");
    xju::assert_equal(xju::format::hex((int)0x143), "0x00000143");
    xju::assert_equal(xju::format::hex((int)0x33fc2143), "0x33fc2143");

    xju::assert_equal(xju::format::hex((long)0x3), "0x00000003");
    xju::assert_equal(xju::format::hex((long)0x43), "0x00000043");
    xju::assert_equal(xju::format::hex((long)0x143), "0x00000143");
    xju::assert_equal(xju::format::hex((long)0x33fc2143), "0x33fc2143");

    xju::assert_equal(xju::format::hex((unsigned char)0x3), "0x03");
    xju::assert_equal(xju::format::hex((unsigned char)0x43), "0x43");

    xju::assert_equal(xju::format::hex((unsigned short)0x3), "0x0003");
    xju::assert_equal(xju::format::hex((unsigned short)0x43), "0x0043");
    xju::assert_equal(xju::format::hex((unsigned short)0x143), "0x0143");
    xju::assert_equal(xju::format::hex((unsigned short)0x2143), "0x2143");

    xju::assert_equal(xju::format::hex((unsigned int)0x3), "0x00000003");
    xju::assert_equal(xju::format::hex((unsigned int)0x43), "0x00000043");
    xju::assert_equal(xju::format::hex((unsigned int)0x143), "0x00000143");
    xju::assert_equal(xju::format::hex((unsigned int)0x33fc2143), "0x33fc2143");
    xju::assert_equal(xju::format::hex((unsigned long)0x3), "0x00000003");
    xju::assert_equal(xju::format::hex((unsigned long)0x43), "0x00000043");
    xju::assert_equal(xju::format::hex((unsigned long)0x143), "0x00000143");
    xju::assert_equal(xju::format::hex((unsigned long)0x33fc2143), "0x33fc2143");
}

// cEscapeChar, cEscapeString
void test8()
{
    xju::assert_equal(xju::format::cEscapeChar('\n'), std::string("\\n"));
    xju::assert_equal(xju::format::cEscapeString("fred\njock"),
                      std::string("fred\\njock"));
}

int main(int argc, char* argv[])
{
    int n = 0;
    test1(); ++n;
    test2(); ++n;
    test3(); ++n;
    test4(); ++n;
    test5(); ++n;
    test6(); ++n;
    test7(); ++n;
    test8(); ++n;
    
    std::cout << "PASS - " << n << " steps" << std::endl;
    return 0;
}
