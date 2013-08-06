// -*- mode: c++; c-file-style: "osse"; -*-
//
//
// From code Copyright (c) 1997 Trevor Taylor
// Copyright (c) 2003 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/Notifying.hh>
#include <xju/Observer.hh>
#include <iostream>
#include <xju/assert.hh>


using namespace xju;


class Client
{
public:
    Client(const Event& event):
	m_count(0),
	m_observer(*this, &Client::trigger, event)
    {
    }
    int m_count;
    void trigger() throw();

    void reset()
    {
	m_count = 0;
    }

private:
    Observer<Client> m_observer;
    
    //
    // Not implemented
    //
    Client& operator=(const Client& b);
};
    
void Client::trigger() throw()
{
    m_count++;
}


void test1()
{
    // constructors, value, assignment

    Notifying<int> y;
    y = 72;
    
    Client z(y.m_changed);
    
    assert_equal(y.value(), 72);
    assert_equal((int)y, 72);
    int v;
    
    v = y = 22;
    assert_equal(v, 22);
    assert_equal(y.value(), 22);
    assert_equal((int)y, 22);
    assert_equal(z.m_count, 1);

    y = 22;
    assert_equal(z.m_count, 1);
    
    Notifying<int> y3;
    Notifying<int> y2 = y3 = y;
    assert_equal(y3.value(), 22);
    assert_equal(y2.value(), 22);
    
    Notifying<int> y4(32);
    assert_equal(y4.value(), 32);
    
    y3 = y2 = y = 15;
    assert_equal((int)y, 15);
    assert_equal((int)y2, 15);
    assert_equal((int)y3, 15);
    
}

int main(int argc, char* argv[])
{
    unsigned int n(0);
    
    test1(); ++n;
    
    std::cout << "Pass - " << n << " steps" << std::endl;
    return 0;
}

    
