//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// From code Copyright (c) 1997 Trevor Taylor with permission
// Copyright (c) 2003 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    This class provides a mechanism to notify a set of observers
//    of an event.  The notification carries a parameter.
//
//    (See also twt_Event.h, which omits the parameter.)
//
#ifndef _XJU_EVENTP_HH_
#define _XJU_EVENTP_HH_


#include <xju/SafeTriggerPIf.hh>
#include <xju/Ring.hh>


namespace xju
{
    
    template<class T>
    class EventP
    {
    public:
	//
	// Postconditions:
	//
	//    m_observers.atomic()
	//
	EventP() throw();
	
	
	//
	// "Trigger" the event.
	//
	// Postconditions:
	//
	//    All m_observers' trigger methods are called exactly once,
	//    with parameter x.
	//
	//    Recursive calls to operator()() are *not* discarded.
	//
	//    If an observer is spliced into observers() during a call to
	//    an observer, the added observer is not called.
	//
	//    The order of calling observers is undefined.
	//
	//    An observer may remove itself or any other observer during its
	//    callback. 
	//
	//    An observer may delete the event during the callback. Any 
	//    observers that have not already been called *will* be
	//    called (so be carefull!)
	//
	void operator()(T x) throw();
	
	
	//
	// Postconditions:
	//
	//    m_observers'.atomic() (i.e. b's observers are not
	//        carried across in the copy)
	//
	//    b.observers()' == b.observers()
	//
	EventP(const EventP& b) throw();
	
	
	//
	// Postconditions:
	//
	//    m_observers' == m_observers (i.e. observers remains the same/b's
	//        observers do not become observers of this)
	//
	//    b.observers()' == b.observers()
	//
	EventP& operator=(const EventP& b) throw();
	
	
	//
	// Preconditions:
	//
	//    (none)
	//
	// Postconditions:
	//
	//    result is the set of observers. (i.e.the set
	//    of objects notified when operator() is called.)
	//
	//    lifetime(result) == lifetime(this)
	//
	Ring<SafeTriggerPIf<T>*>& observers() const throw();
	
	
    private:
	//
	// (Should be mutable, but not many compilers
	// support, so work-around method is provided.)
	//
	Ring<SafeTriggerPIf<T>*> m_observers;
	
	
	
	class Nl : public SafeTriggerPIf<T>
	{
	public:
	    //
	    // SafeTriggerPIf<T>::callback()
	    //
	    void trigger(T) throw()
	    {
	    }
	};
    };
    
    
    template<class T>
    inline EventP<T>::EventP() throw():
	m_observers(0)
    {
    }
    
    
    template<class T>
    inline EventP<T>::EventP(const EventP& b) throw():
	m_observers(0)
    {
    }
    
    
    template<class T>
    inline EventP<T>& EventP<T>::operator=(const EventP& b) 
	throw()
    {
	return *this;
    }
    
    
    template<class T>
    inline void EventP<T>::operator()(T x) throw()
    {
	Nl nl;
	
	
	//
	// We use nl rather than 0 to allow recursive
	// invocation.
	//
	Ring<SafeTriggerPIf<T>*> start(&nl);
	Ring<SafeTriggerPIf<T>*> end(&nl);
	
	m_observers.splice(start);
	m_observers.prev().splice(end);
	
	while(start.next() != end)
	{
	    start.swapWithNext();
	    start.prev().m_value->trigger(x);
	}
    }
    
    
    template<class T>
    inline Ring<SafeTriggerPIf<T>*>& EventP<T>::observers() const
	throw()
    {
	//
	// Work around lack of mutable.
	//
	return *(Ring<SafeTriggerPIf<T>*>*)&m_observers;
    }
}

#endif
    
