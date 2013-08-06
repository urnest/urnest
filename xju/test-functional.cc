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
#include "xju/functional.hh"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "xju/assert.hh"
#include "xju/Shared.hh"
#include <functional>
#include <utility>

void test1()
{
    std::string const a("a");
    std::vector<std::string const*> x;
    x.push_back(&a);
    std::vector<size_t> y;
    std::transform(
        x.begin(), 
        x.end(),
        std::back_inserter(y),
        xju::functional::deref(xju::functional::method(&std::string::size)));
    xju::assert_equal(y, std::vector<size_t>(1, 1U));
}

void test2()
{
    std::string const a("a");
    std::vector<std::string> x;
    x.push_back(a);
    std::vector<size_t> y;
    std::transform(
        x.begin(), 
        x.end(),
        std::back_inserter(y),
        xju::functional::method(&std::string::size));
    xju::assert_equal(y, std::vector<size_t>(1, 1U));
}

void test3()
{
    std::vector<xju::Shared<std::string const> > x;
    x.push_back(xju::Shared<std::string const>(new std::string("a")));
    std::vector<size_t> y;
    std::transform(
        x.begin(), 
        x.end(),
        std::back_inserter(y),
        xju::functional::deref(xju::functional::method(&std::string::size)));
    xju::assert_equal(y, std::vector<size_t>(1, 1U));
}

int f4(int x)
{
    return x+1;
}

void test4()
{
    std::vector<xju::Shared<int> > x;
    x.push_back(new int(1));
    x.push_back(new int(2));
    
    std::vector<int> expect;
    expect.push_back(2);
    expect.push_back(3);
    
    std::vector<int> result;
    std::transform(x.begin(), x.end(), std::back_inserter(result),
                   xju::functional::deref(std::ptr_fun(f4)));
    xju::assert_equal(result, expect);
    
}

int main(int argc, char* argv[])
{
    int n = 0;
    test1(); ++n;
    test2(); ++n;
    test3(); ++n;
    test4(); ++n;
    
    std::cout << "PASS - " << n << " steps" << std::endl;
    return 0;
}
