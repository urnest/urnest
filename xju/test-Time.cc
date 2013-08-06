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
#include <xju/Time.hh>


#include <xju/assert.hh>
#include <functional>
#include <sys/types.h>
#include <iostream>

namespace
{
    bool notEarlier(const xju::Time& a, const xju::Time& b) throw()
    {
	return !(a < b);
    }
}

//
// the addition and subtraction algorithms have only been
// verified where unsigned int is at least 32 bits
//
void test1()
{
    xju::assert_(sizeof(unsigned int), std::greater_equal<size_t>(), 4);
}


void test2()
{
    const xju::Time t1(1, 2);
    xju::assert_equal(t1.getUsecs(), 2U);
    xju::assert_equal(t1.getSecs(), 1U);
    
    xju::assert_(t1, notEarlier, t1);
    xju::assert_equal(t1, t1);
    
    const xju::Time t2(1U, 3U);
    xju::assert_(t1, std::less<xju::Time>(), t2);
    
    const xju::Time t3(2U, 1U);
    xju::assert_(t1, std::less<xju::Time>(), t3);
    
    const xju::Time t4(2U, 4U);
    xju::assert_(t1, std::less<xju::Time>(), t4);
    
    const xju::Time t5(t1);
    xju::assert_equal(t5, t1);
    
    xju::Time t6(t2);
    t6 = t1;
    xju::assert_equal(t6, t1);
}


// addition
void test3()
{
    const xju::Time t1(1U, 999998U);
    const xju::MicroSeconds d(1U);
    
    xju::assert_equal(t1+d, xju::Time(1U, 999999U));
    xju::assert_equal(t1+d+d, xju::Time(2U, 0U));
    
    const xju::MicroSeconds d3(4000005U);
    xju::assert_equal(t1+d3, xju::Time(6U, 3U));
}


// subtraction
void test4()
{
    const xju::Time t1(5U, 5U);
    
    xju::assert_equal(t1-xju::MicroSeconds(5000005U), xju::Time(0U, 0U));
    xju::assert_equal(t1-xju::MicroSeconds(4000004U), xju::Time(1U, 1U));
    xju::assert_equal(t1-xju::MicroSeconds(4000006U), xju::Time(0U, 999999U));
}


int main(int argc, char* argv[])
{
    unsigned int n(0);
    test1(); ++n;
    test2(); ++n;
    test3(); ++n;
    test4(); ++n;
    
    std::cout << "PASS - " << n << " steps" << std::endl;
    return 0;
}
