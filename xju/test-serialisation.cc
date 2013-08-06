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
#include <xju/serialisation.hh>

#include <iostream>

#include <xju/assert.hh>

void test1()
{
    xju::assert_equal(sizeof(char), 1);
    xju::assert_equal(sizeof(short), 2);
    xju::assert_equal(sizeof(int), 4);
}

void test2()
{
    unsigned short x;
    unsigned char* p((unsigned char*)&x);
    p[0] = 1;
    p[1] = 0;
    xju::serialisation::toHostOrder(x);
    xju::assert_equal(x, 1);
    
    unsigned short y = x;
    xju::serialisation::fromHostOrder(y);
    p = ((unsigned char*)&y);
    
    xju::assert_equal(p[0], 1);
    xju::assert_equal(p[1], 0);
}

void test3()
{
    char c(55);
    xju::serialisation::fromHostOrder(c);
    xju::serialisation::toHostOrder(c);
    xju::assert_equal(c, (char)55);
    
    short s(1);
    xju::serialisation::fromHostOrder(s);
    xju::serialisation::toHostOrder(s);
    xju::assert_equal(s, (short)1);
    
    int i(0x01020304);
    xju::serialisation::fromHostOrder(i);
    xju::serialisation::toHostOrder(i);
    xju::assert_equal(i, 0x01020304);
}

int main(int argc, char* argv[])
{
    unsigned int n(0);
    
    test1(); ++n;
    test2(); ++n;
    test3(); ++n;
    
    std::cout << "PASS - " << n << " steps" << std::endl;
    return 0;
}

