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
//    This class provides a mechanism to notify a set of observers
//    of an event.
//
//    (See also EventP.hh, which carries a parameter 
//    with the notification.)
//
#ifndef _XJU_EVENT_H_
#define _XJU_EVENT_H_


#include <xju/Ring.hh>
#include <xju/SafeTriggerIf.hh>


namespace xju
{
    class Event
    {
    public:
	//
	// Postconditions:
	//
	//    m_observers.atomic()
	//
	Event() throw();
	
	
	//
	// "Trigger" the event.
	//
	// Postconditions:
	//
	//    All m_observers' trigger methods are called exactly once.
	//
	//    Recursive calls to operator()() are *not* discarded.
	//
	//    If an observer is added during a call to an observer, the added 
	//    observer is not called.
	//
	//    The order of calling observers is undefined.
	//
	//    An observer may remove itself or any other observer during its
	//    callback. 
	//
	//    An observer may delete the event during the callback. Any 
	//    observers that have not already been called *will* be called
	//    (so be carefull!)
	//
	void operator()() 
	    throw();
	
	
	//
	// Postconditions:
	//
	//    m_observers'.atomic() (i.e. b's observers are not
	//    carried across in the copy)
	//
	//    b.observers()' == b.observers()
	//
	Event(const Event& b) 
	    throw();
	
	
	//
	// Postconditions:
	//
	//    m_observers' == m_observers (i.e. observers remain the 
	//    same/b's observers do not become observers of this)
	//
	//    b.observers()' == b.observers()
	//
	Event& operator=(const Event& b) 
	    throw();
	
	
	
	//
	// Preconditions:
	//
	//    (none)
	//
	// Postconditions:
	//
	//    The set of observers. (i.e.the set
	//    of objects notified when operator() is called.)
	//
	//    result == m_observers
	//    result.m_value == 0
	//
	//    lifetime(result) == lifetime(this)
	//
	const Ring<SafeTriggerIf*>& observers() const throw();
	
	
	const Ring<SafeTriggerIf*> m_observers;
    };
    
    
    inline Event::Event() throw():
	m_observers(0)
    {
    }
    
    
    inline Event::Event(const Event& b) throw():
	m_observers(0)
    {
    }
    
    
    inline Event& Event::operator=(const Event& b) 
	throw()
    {
	return *this;
    }
    
    
    class EventNl : public SafeTriggerIf
    {
    public:
	//
	// SafeTriggerIf::callback()
	//
	void trigger() throw()
	{
	}
    };
    
    inline void Event::operator()() throw()
    {
	EventNl nl;
	
	
	//
	// We use nl rather than 0 to allow recursive
	// invocation.
	//
	Ring<SafeTriggerIf*> start(&nl);
	Ring<SafeTriggerIf*> end(&nl);
	
	m_observers.splice(start);
	m_observers.prev().splice(end);
	
	while(start.next() != end)
	{
	    start.swapWithNext();
	    start.prev().m_value->trigger();
	}
    }
    
    
    inline const Ring<SafeTriggerIf*>& Event::observers() const 
	throw()
    {
	return m_observers;
    }
    
}

#endif
    
