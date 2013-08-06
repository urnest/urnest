//     -*- mode: c++ ; c-file-style: "osse" ;  -*-
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
#include <xju/Traced.hh>


#include <xju/assert.hh>
#include <string>
#include <iostream>

using namespace xju;

void test1()
{
    const xju::Traced x = XJU_TRACED;
    xju::assert_equal(x.line(), 0U+__LINE__-1);
    xju::assert_equal(x.file().size(), std::string(__FILE__).size());
    
    const std::string::const_reverse_iterator slash(std::find(
	x.file().rbegin(),
	x.file().rend(),
	'/'));
    const std::string::size_type offset(slash - x.file().rbegin());
    const std::string fileName(x.file().begin()+(x.file().size() - offset),
			       x.file().end());
    xju::assert_equal(fileName, "test-Traced.cc");
}


int main(int argc, char* argv[])
{
    unsigned int n(0);
    test1(); ++n;
    
    std::cout << "PASS - " << n << " steps" << std::endl;
    return 0;
}
