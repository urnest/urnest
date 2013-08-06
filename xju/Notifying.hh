//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// From code Copyright (c) 1997 Trevor Taylor with permission
// Copyright (c) 2002 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    A template class that contains an object, and generates
//    an event when that object is changed.
//
#ifndef _XJU_NOTIFYING_HH_
#define _XJU_NOTIFYING_HH_


#include <xju/Event.hh>

namespace xju
{
    //
    // Preconditions:
    //
    //    T has:
    //
    //        T(const T&)
    //        const T& operator=(const T&)
    //        int operator!=(const T&) const
    //
    template<class T>
    class Notifying
    {
    public:
	//
	// Postconditions:
	//
	//    value() == T()
	//
	Notifying();
	
	//
	// Postconditions:
	//
	//    value() == x
	//
	Notifying(const T& x);
	
	
	//
	// Postconditions:
	//
	//    value() == b.value()
	//
	Notifying(const Notifying& b);
	
	
	//
	// Postconditions
	//
	//    value()' == x
	//    value()' == value() || m_changed occurrs once,
	//        after value() changes
	//
	const Notifying& operator=(const T& x);
	
	
	//
	// Postconditions
	//
	//    value()' == b.value()
	//    value()' == value() || m_changed occurrs once,
	//        after value() changes
	//
	const Notifying& operator=(const Notifying& b);
	
	
	//
	// The event that is triggered whenever
	// x (the wrapped object) is changed via
	// the wrapper's operator=()
	//
	// This event always occurs after the value
	// has changed, so a call to value() by a
	// client of the event will return the new value.
	//
	Event m_changed;
	
	
	//
	// Report the value of the contained object
	//
	const T& value() const;
	operator const T&() const;
	
    private:
	T m_x;
    };
    
    
    template<class T>
    Notifying<T>::Notifying()
    {
    }
    
    
    template<class T>
    Notifying<T>::Notifying(const T& x):
	m_x(x)
    {
    }
    
    template<class T>
    Notifying<T>::Notifying(const Notifying& b):
	m_x(b.value())
    {
    }
    
    template<class T>
    const Notifying<T>& Notifying<T>::operator=(const T& x)
    {
	if (m_x != x)
	{
	    m_x = x;
	    m_changed();
	}
	return *this;
    }
    
    
    template<class T>
    const Notifying<T>& Notifying<T>::operator=(const Notifying& b)
    {
	if (this != &b)
	{
	    if (value() != b.value())
	    {
		operator=(b.value());
	    }
	}
	return *this;
    }
    
    template<class T>
    const T& Notifying<T>::value() const
    {
	return m_x;
    }
    
    template<class T>
    Notifying<T>::operator const T&() const
    {
	return m_x;
    }
}


#endif
