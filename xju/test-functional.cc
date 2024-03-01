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
#include "xju/assert.hh"
#include <functional>
#include <utility>

void test1()
{
    std::pair<int, std::string> const x(87, "fred");
    xju::assert_equal(xju::functional::first(x), 87);
    xju::assert_equal(xju::functional::second(x), std::string("fred"));
}

int main(int argc, char* argv[])
{
    int n = 0;
    test1(); ++n;
    
    std::cout << "PASS - " << n << " steps" << std::endl;
    return 0;
}
