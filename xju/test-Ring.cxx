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
// From code Copyright (c) 1997 Trevor Taylor
//
#include <xju/Ring.hh>
#include <xju/assert.hh>
#include <iostream>


using namespace xju;


void test1()
{
    Ring<int> a(1);
    
    assert_equal(a.atomic(), true);
    assert_equal(&a.next(), &a.prev());
    assert_equal(a.m_value, 1);

    Ring<int> b(a);
    assert_equal(a.atomic(), true);
    assert_equal(&a.next(), &a.prev());
    assert_equal(a.m_value, 1);
    assert_equal(b.atomic(), true);
    assert_equal(&b.next(), &b.prev());
    assert_equal(b.m_value, 1);

    b = 2;
    assert_equal(b.atomic(), true);
    assert_equal(&b.next(), &b.prev());
    assert_equal(b.m_value, 2);

    a.splice(b);
    assert_equal(a.atomic(), false);
    assert_equal(a.m_value, 1);
    assert_equal(b.atomic(), false);
    assert_equal(b.m_value, 2);
    assert_equal(&a.next(), &b);
    assert_equal(&a.prev(), &b);
    assert_equal(&b.next(), &a);
    assert_equal(&b.prev(), &a);

    b.cut();
    assert_equal(a.atomic(), true);
    assert_equal(a.m_value, 1);
    assert_equal(&a.next(), &a.prev());
    assert_equal(b.atomic(), true);
    assert_equal(b.m_value, 2);
    assert_equal(&b.next(), &b.prev());

    a.splice(b);

    Ring<int> c(3);
    b.splice(c);
    assert_equal(c.atomic(), false);
    assert_equal(c.m_value, 3);
    assert_equal(&b.next(), &c);
    assert_equal(&c.prev(), &b);
    assert_equal(&a.prev(), &c);
    assert_equal(&c.next(), &a);
    assert_equal(a.next(), b);
    assert_equal(b.next(), c);
    assert_equal(c.next(), a);

    b.swapWithNext();
    
    assert_equal(c.prev(), a);
    assert_equal(c.next(), b);
    assert_equal(b.next(), a);
    assert_equal(&a.prev(), &b);
    assert_equal(c.prev(), a);
    assert_equal(a.m_value, 1);
    assert_equal(b.m_value, 2);
    assert_equal(c.m_value, 3);

    a.m_value = 5;
    assert_equal(a.m_value, 5);

}


void test2()
{
    int x = 2;
    Ring<int*> r(&x);
    assert_equal(r.m_value, &x);
}


int main(int argc, char* argv[])
{
    unsigned int n(0);
    
    test1(); ++n;
    test2(); ++n;
    
    std::cout << "Pass - " << n << " steps" << std::endl;
    return 0;
}
