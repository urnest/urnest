//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// From code Copyright (c) 1998 Trevor Taylor with permission
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    Observer for twt_Event events.
//
//    Watches for the occurrance of a specified twt_Event and,
//    when it occurs, triggers a specified method on a specified
//    object.
//
#ifndef _XJU_OBSERVER_HH_
#define _XJU_OBSERVER_HH_


#include <xju/MethodTrigger.hh>
#include <xju/Ring.hh>
#include <xju/SafeTriggerIf.hh>
#include <xju/Event.hh>


namespace xju
{
    template<class T>
    class Observer
    {
	typedef void (T::*Method)();
	
    public:
	//
	// Preconditions:
	//
	//    x.f() is the method to trigger when target occurs
	//
	//    lifetime(x) > lifetime(this)
	//    lifetime(f) > lifetime(this)
	//
	// Postconditions:
	//
	//    x.f() will be called once for every occurrance of target
	//    until this is deleted
	//
	Observer(T& x, Method f, const Event& target):
	    m_x(x, f),
	    m_r(&m_x)
	{
	    target.observers().splice(m_r);
	}
	
	
	//
	// Postconditions:
	//
	//    this observes target
	//
	Observer& operator=(const Event& target)
	{
	    m_r.cut();
	    target.observers().splice(m_r);
	    return *this;
	}
	
	
	//
	// Postconditions:
	//
	//    this observes whatever b is observing
	//
	Observer(T& x, Method f, const Observer& b):
	    m_x(x, f),
	    m_r(&m_x)
	{
	    b.m_r.splice(m_r);
	}
	
	
	//
	// Postconditions:
	//
	//    this observes whatever x is observing
	//
	Observer& operator=(const Observer& x)
	{
	    if (this != &x)
	    {
		m_r.cut();
		x.m_r.splice(m_r);
	    }
	    return *this;
	}
	
	
    private:
	MethodTrigger<T, SafeTriggerIf> m_x;
	Ring<SafeTriggerIf*> m_r;
    };
    
}

#endif
    
