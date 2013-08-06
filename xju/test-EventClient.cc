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
#include <xju/EventClient.hh>
#include <xju/Event.hh>
#include <iostream>
#include <xju/assert.hh>

using namespace xju;

class Client : private EventClient
{
public:
    Client(const Event& event):
	EventClient(event),
	m_count(0)
    {
    }
    int m_count;
    virtual void trigger() throw();

    void reset()
    {
	m_count = 0;
    }
    Client& operator=(const Event& event)
    {
	EventClient::operator=(event);
	return *this;
    }
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
}

class Client3 : public EventClient
{
public:
    static int m_deleted;
    static int m_called;

    Client3(Event& event):
	EventClient(event)
    {
    }
    
    virtual void trigger() throw();

    ~Client3()
    {
	m_deleted++;
    }
};

int Client3::m_deleted = 0;
int Client3::m_called = 0;

    
void Client3::trigger() throw()
{
    m_called++;
}
    

class Client2 : public EventClient
{
public:
    int m_count;
    Client3* m_toDelete;
    
    Client2(Event& event, Client3* toDelete):
	EventClient(event),
	m_count(0),
	m_toDelete(toDelete)
    {
    }
    virtual void trigger() throw();

    void reset()
    {
	m_count = 0;
    }
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

void test3()
{
    
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

class Client4 : public EventClient
{
public:
    int m_count;
    Client* m_client;
    Event& m_event;
    
    Client4(Event& event):
	EventClient(event),
	m_count(0),
	m_client(0),
	m_event(event)
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

class Client5 : public EventClient
{
public:
    int m_count;
    int m_inProgress;
    Event& m_event;
    
    Client5(Event& event):
	EventClient(event),
	m_count(0),
	m_inProgress(0),
	m_event(event)
    {
    }
    void trigger() throw();
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
    Event event;
    Client c1(event);
    Client5 c2(event);
    event();
    assert_equal(c1.m_count, 2);
    assert_equal(c2.m_count, 2);
}

class Client6 : public EventClient
{
public:
    Event* m_event;
    Client& m_c1;
    Client* m_c2;
    int m_count;
    
    Client6(Event* event,
	    Client& c1):
	EventClient(*event),
	m_c1(c1),
	m_c2(0),
	m_count(0)
    {
    }
    void setC2(Client& c2)
    {
	m_c2 = &c2;
    }
    
    virtual void trigger() throw();

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
    // test deletion of event source during trigger"
    
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

    
int main(int argc, char* argv[])
{
    unsigned int n(0);
    
    test1(); ++n;
    test3(); ++n;
    test4(); ++n;
    test5(); ++n;
    test6(); ++n;
    
    std::cout << "Pass - " << n << " steps" << std::endl;
    return 0;
}

    
