//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// From code Copyright (c) 1997 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#include <xju/Observer.hh>
#include <xju/Event.hh>
#include <xju/assert.hh>
#include <iostream>


using namespace xju;


class Client
{
public:
    Client(const Event& event):
	m_count(0),
	m_observer(*this, &Client::trigger, event)
    {
    }
    Client(const Client& b):
	m_count(0),
	m_observer(*this, &Client::trigger, b.m_observer)
    {
    }
    

    int m_count;
    void trigger() throw();

    void reset()
    {
	m_count = 0;
    }

    void retarget(const Event& event)
    {
	m_observer = event;
    }
    void retarget(const Client& b)
    {
	m_observer = b.m_observer;
    }
    

private:
    Observer<Client> m_observer;
};
    
void Client::trigger() throw()
{
    m_count++;
}


void test1()
{
    // constructors, triggering, assignment

    Event event;

    event();
    
    Client c1(event);
    
    event();
    assert_equal(c1.m_count, 1);
    
    Client c2(event);
    event();
    assert_equal(c1.m_count, 2);
    assert_equal(c2.m_count, 1);
    

    event();
    assert_equal(c1.m_count, 3);
    assert_equal(c2.m_count, 2);
    
    
    Event event2;
    Client c3(event2);

    event();
    assert_equal(c1.m_count, 4);
    assert_equal(c2.m_count, 3);
    assert_equal(c3.m_count, 0);
    

    Event e2;
    
    c3.reset();
    c3 = e2;
    event();
    assert_equal(c1.m_count, 5);
    assert_equal(c2.m_count, 4);
    assert_equal(c3.m_count, 0);
    

    e2();
    assert_equal(c1.m_count, 5);
    assert_equal(c2.m_count, 4);
    assert_equal(c3.m_count, 1);


    Client c4(c3);

    e2();
    assert_equal(c1.m_count, 5);
    assert_equal(c2.m_count, 4);
    assert_equal(c3.m_count, 2);
    assert_equal(c4.m_count, 1);
    

    c2.retarget(c4);
    e2();
    assert_equal(c1.m_count, 5);
    assert_equal(c2.m_count, 5);
    assert_equal(c3.m_count, 3);
    assert_equal(c4.m_count, 2);
}


class Client3
{
public:
    static int m_deleted;
    static int m_called;

    Client3(Event& event):
	m_observer(*this, &Client3::trigger, event)
    {
    }
    
    void trigger() throw();

    ~Client3()
    {
	m_deleted++;
    }
private:
    Observer<Client3> m_observer;
};

int Client3::m_deleted = 0;
int Client3::m_called = 0;

    
void Client3::trigger() throw()
{
    m_called++;
}
    

class Client2
{
public:
    int m_count;
    Client3* m_toDelete;
    
    Client2(Event& event, Client3* toDelete):
	m_count(0),
	m_toDelete(toDelete),
	m_observer(*this, &Client2::trigger, event)
    {
    }
    void trigger() throw();

    void reset()
    {
	m_count = 0;
    }
private:
    Observer<Client2> m_observer;
};

void Client2::trigger() throw()
{
    m_count++;
    if (m_toDelete)
    {
	delete m_toDelete;
	m_toDelete = 0;
    }
}

void test2()
{
    // test deletion during trigger
    
    Event event;
    Client2 c1(event, new Client3(event));
    
    event();
    assert_equal(c1.m_count, 1);
    assert_equal(Client3::m_deleted, 1);
    int x = Client3::m_called;
    
    event();
    assert_equal(c1.m_count, 2);
    assert_equal(Client3::m_deleted, 1);
    assert_equal(Client3::m_called, x);
}

class Client4
{
public:
    int m_count;
    Client* m_client;
    Event& m_event;
    
    Client4(Event& event):
	m_count(0),
	m_client(0),
	m_event(event),
	m_observer(*this, &Client4::trigger, event)
    {
    }
    void trigger() throw();
    
    ~Client4()
    {
	if (m_client)
	{
	    delete m_client;
	    m_client = 0;
	}
    }
private:
    Observer<Client4> m_observer;
};

void Client4::trigger() throw()
{
    m_count++;
    if (!m_client)
    {
	m_client = new Client(m_event);
    }
}

void test4()
{
    // add a client during trigger, check it isn't called
    
    Event event;
    Client4 c1(event);
    
    event();
    assert_equal(c1.m_count, 1);
    assert_not_equal(c1.m_client, (Client*)0);
    assert_equal(c1.m_client->m_count, 0);
    
    event();
    assert_equal(c1.m_count, 2);
    assert_not_equal(c1.m_client, (Client*)0);
    assert_equal(c1.m_client->m_count, 1);
}

class Client5
{
public:
    int m_count;
    Event& m_event;
    int m_inProgress;
    
    Client5(Event& event):
	m_count(0),
	m_event(event),
	m_inProgress(0),
	m_observer(*this, &Client5::trigger, event)
    {
    }
    void trigger() throw();
private:
    Observer<Client5> m_observer;
};
void Client5::trigger() throw()
{
    ++m_count;
    
    if ((m_count == 2) && !m_inProgress)
    {
	abort();
    }
    m_inProgress = 1;
    if (m_count == 1)
    {
	m_event();
    }
    m_inProgress = 0;
}


void test5()
{
    // recursive trigger - check clients called correct number of times
    
    Event event;
    Client c1(event);
    Client5 c2(event);
    event();
    assert_equal(c1.m_count, 2);
    assert_equal(c2.m_count, 2);
}

class Client6
{
public:
    Client& m_c1;
    Client* m_c2;
    int m_count;
    
    Client6(Event* event,
	    Client& c1):
	m_c1(c1),
	m_c2(0),
	m_count(0),
	m_observer(*this, &Client6::trigger, *event)
    {
    }
    void setC2(Client& c2)
    {
	m_c2 = &c2;
    }
    
    void trigger() throw();

private:
    Observer<Client6> m_observer;
};
void Client6::trigger() throw()
{
    if ((m_c1.m_count + m_c2->m_count) != 1)
    {
	abort();
    }
    ++m_count;
}


void test6()
{
    // test deletion of event source during trigger
    
    Event* event = new Event();
    Client c1(*event);
    Client6 c2(event, c1);
    Client c3(*event);
    
    c2.setC2(c3);
    
    (*event)();
    assert_equal(c1.m_count, 1);
    assert_equal(c2.m_count, 1);
    assert_equal(c3.m_count, 1);
}


void test7()
{
    // test retarget of event source
    
    Event s1;
    Client c(s1);

    s1();
    assert_equal(c.m_count, 1);
    
    Event s2;
    c.retarget(s2);
    s1();
    assert_equal(c.m_count, 1);
    
    s2();
    assert_equal(c.m_count, 2);
}


int main(int argc, char* argv[])
{
    unsigned int n(0);
    
    test1(); ++n;
    test2(); ++n;
    test4(); ++n;
    test5(); ++n;
    test6(); ++n;
    test7(); ++n;
    
    std::cout << "Pass - " << n << " steps" << std::endl;
    return 0;
}

    
