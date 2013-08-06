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
#include <xju/JoiningIterator.hh>


#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <xju/assert.hh>
#include <vector>

using namespace xju;

void test1()
{
    std::vector<int> x;
    std::ostringstream s;
    
    std::copy(x.begin(),
	      x.end(),
	      xju::JoiningIterator<int, std::string>(s, std::string(", ")));
    xju::assert_equal(s.str(), std::string(""));
}


void test2()
{
    std::vector<int> x;
    x.push_back(2);
    
    std::ostringstream s;
    
    std::copy(x.begin(),
	      x.end(),
	      xju::JoiningIterator<int, std::string>(s, ", "));
    xju::assert_equal(s.str(), std::string("2"));
}


void test3()
{
    std::vector<int> x;
    x.push_back(1);
    x.push_back(2);
    x.push_back(3);
    
    std::ostringstream s;
    
    std::copy(x.begin(), x.end(), xju::JoiningIterator<int, std::string>(s, ", "));
    xju::assert_equal(s.str(), std::string("1, 2, 3"));
}


int main(int argc, char* argv[])
{
    unsigned int n = 0;
    test1(); ++n;
    test2(); ++n;
    test3(); ++n;
    std::cout << "PASS - " << n << " steps" << std::endl;
    return 0;
}
