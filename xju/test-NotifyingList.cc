//     -*- mode: c++ ; c-file-style: "osse" ; -*-
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
#include <xju/NotifyingList.hh>
#include <list>
#include <xju/EventP.hh>
#include <deque>
#include <xju/Observer.hh>
#include <xju/ObserverP.hh>
#include <iostream>
#include <xju/assert.hh>

using namespace xju;

typedef NotifyingList<int> ListType;

class RemoveWatcher
{
public:
    static int m_count;
    typedef std::deque<int> Deleted;
    
    static Deleted m_deleted;
    
    ListType::iterator m_i;
    
    RemoveWatcher(ListType::iterator i, const Event& removing):
	m_i(i),
	m_removed(*this, &RemoveWatcher::trigger, removing)
    {
    }
    void trigger() throw();
    
    
private:
    Observer<RemoveWatcher> m_removed;
};
int RemoveWatcher::m_count = 0;
RemoveWatcher::Deleted RemoveWatcher::m_deleted;

void RemoveWatcher::trigger() throw()
{
    ++m_count;
    m_deleted.push_back(*m_i);
    delete(this);
}

struct Counters
{
    Counters():
	m_added(0),
	m_removed(0)
    {
    }
    void added(int i) throw()
    {
	++m_added;
	m_addedItems.push_back(i);
    }
    void removed(int i) throw()
    {
	++m_removed;
	m_removedItems.push_back(i);
    }
    
    unsigned int m_added;
    unsigned int m_removed;
    std::deque<int> m_addedItems;
    std::deque<int> m_removedItems;
};


class ItemWatcher
{
public:
    //
    // i is the item to watch within list l
    //
    ItemWatcher(const ListType& l,
		const ListType::const_iterator i,
		Counters& counters) throw():
	m_l(l),
	m_i(i),
	m_counters(counters),
	m_newPredecessor(
	    *this,
	    &ItemWatcher::newPredecessor, 
	    l.newPredecessor(i)),
	m_removing(*this, &ItemWatcher::removing, l.removing(i))
    {
	m_counters.added(*i);
    }
    void newPredecessor() throw()
    {
	ListType::const_iterator newi(m_i); --newi;
	new ItemWatcher(m_l, newi, m_counters);
    }
    void removing() throw()
    {
	m_counters.removed(*m_i);
	delete this;
    }
    
private:
    const ListType& m_l;
    const ListType::const_iterator m_i;
    Counters& m_counters;
    
    Observer<ItemWatcher> m_newPredecessor;
    Observer<ItemWatcher> m_removing;

    // not implemented
    ItemWatcher(const ItemWatcher&);
    ItemWatcher& operator=(ItemWatcher&);
};


class FirstWatcher
{
public:
    FirstWatcher(const ListType& l, Counters& counters) throw():
	m_l(l),
	m_counters(counters),
	m_firstAdded(*this,
		     &FirstWatcher::firstAdded,
		     l.newPredecessor(l.end()))
    {
    }

    void firstAdded() throw()
    {
	ListType::const_iterator i(m_l.end());
	--i;
	new ItemWatcher(m_l, i, m_counters);
    }
private:
    const ListType& m_l;
    Counters& m_counters;
    Observer<FirstWatcher> m_firstAdded;
};


void test1()
{
    // test insert/erase events
    Counters counters;
    
    NotifyingList<int> items;
    FirstWatcher firstWatcher(items, counters);
    
    items.insert(items.end(), 1);
    assert_equal(counters.m_added, 1U);
    assert_equal(counters.m_addedItems.front(), 1);
    counters.m_addedItems.pop_front();
    assert_equal(counters.m_removed, 0U);
    
    items.erase(items.begin());
    assert_equal(counters.m_added, 1U);
    assert_equal(counters.m_removed, 1U);
    assert_equal(counters.m_removedItems.back(), 1);
    counters.m_removedItems.pop_back();
}

void test2()
{
    // test multiple insert/erase events

    NotifyingList<int> items;

    Counters counters;
    FirstWatcher firstWatcher(items, counters);
    
    items.insert(items.end(), 1);
    assert_equal(counters.m_added, 1U);
    assert_equal(counters.m_removed, 0U);
    

    items.insert(items.begin(), 10);
    assert_equal(counters.m_added, 2U);
    assert_equal(counters.m_removed, 0U);
    

    items.insert(items.end(), 20);
    assert_equal(counters.m_added, 3U);
    assert_equal(counters.m_removed, 0U);
    assert_equal(items.size(), 3U);
    
    NotifyingList<int>::iterator i = items.begin();
    ++i;
    assert_equal(*i, 1);
    
    items.erase(i);
    assert_equal(counters.m_added, 3U);
    assert_equal(counters.m_removed, 1U);
    assert_equal(items.size(), 2U);
    assert_equal(*items.begin(), 10);
    
    items.erase(items.begin());
    assert_equal(counters.m_added, 3U);
    assert_equal(counters.m_removed, 2U);
    assert_equal(items.size(), 1U);
    assert_equal(*items.begin(), 20);
}

void test3()
{
    // test mute()

    NotifyingList<int> items;
    items.insert(items.end(), 1);
    
    NotifyingList<int>::const_iterator i(items.begin());
    NotifyingList<int>::iterator j(items.mute(i));
    (*j) = 2;
    xju::assert_equal(*items.begin(), 2);
}

int main(int argc, char* argv[])
{
    unsigned int n(0);
    
    test1(); ++n;
    test2(); ++n;
    test3(); ++n;
    
    std::cout << "Pass - " << n << " steps" << std::endl;
    return 0;
}
