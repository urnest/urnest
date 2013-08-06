//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// From code Copyright (c) 1998 Trevor Taylor with permission
// Copyright (c) 2003 Trevor Taylor
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
//    The event has an arbitrary parameter that is passed to the 
//    client.
//
#ifndef _XJU_OBSERVERP_HH_
#define _XJU_OBSERVERP_HH_


#include <xju/MethodTriggerP.hh>
#include <xju/Ring.hh>
#include <xju/SafeTriggerPIf.hh>
#include <xju/EventP.hh>


namespace xju
{
    template<class T, class P>
    class ObserverP
    {
	typedef void (T::*Method)(P);
	
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
	ObserverP(T& x, Method f, const EventP<P>& target):
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
	ObserverP& operator=(const EventP<P>& target)
	{
	    m_r.cut();
	    target.observers().splice(m_r);
	    return* this;
	}
	
	
	//
	// Postconditions:
	//
	//    this observes whatever x is observing
	//
	ObserverP(T& x, Method f, const ObserverP& b):
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
	ObserverP& operator=(const ObserverP& x)
	{
	    if (this != &x)
	    {
		m_r.cut();
		x.m_r.splice(m_r);
	    }
	    return* this;
	}
	
	
    private:
	MethodTriggerP<T, SafeTriggerPIf<P>, P> m_x;
	Ring<SafeTriggerPIf<P>*> m_r;
    };
    
}

#endif
    
