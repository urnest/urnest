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
//    Class that holds a value of a specified type and
//    can be part of a ring of objects of this class.
//
#ifndef _XJU_RING_H_
#define _XJU_RING_H_


namespace xju
{
    
    //
    // Preconditions:
    //
    //    (none)
    //
    // Postconditions:
    //
    //    (none)
    //
    template<class T>
    class Ring
    {
    public:
	typedef T value_type;
	
	T m_value;
	
	
	//
	// Determine whether this link is attached to any
	// others.
	//
	// Postconditions:
	//
	//    Returns non-zero if this link is not attached to
	//    any others, zero if it is.
	//
	int atomic() const throw();
	
	
	//
	// Postconditions:
	//
	//    atomic()
	//    m_value == T
	//
	Ring(T);
	
	
	//
	// Copy the value (i.e. the contained object) of
	// another ring.
	// 
	// Preconditions:
	// 
	//    T has copy constructor
	//
	// Postconditions:
	//
	//    m_value == ring.m_value
	//    atomic()
	//    ring.next()' == ring.next()
	//    ring.prev()' == ring.prev()
	//
	Ring(const Ring& ring);
	
	
	//
	// Assign the value of x to the contained object
	// 
	// Preconditions:
	// 
	//    T has operator=(const T&)
	//
	// Postconditions:
	//
	//    m_value == ring.m_value
	//    next()' == next()
	//    prev()' == prev()
	//    x.next()' == x.next()
	//    x.prev()' == x.prev()
	//
	const Ring& operator=(const Ring& x);
	
	
	//
	// Assign to the contained object
	// 
	// Preconditions:
	// 
	//    T has operator=(const T&)
	//
	// Postconditions:
	//
	//    m_value == ring.m_value
	//    next()' == next()
	//    prev()' == prev()
	//
	const Ring& operator=(const T& x);
	
	
	//
	// Return the element after this.
	// 
	// Preconditions:
	// 
	//    None
	//
	// Postconditions:
	//
	//    returns *this || !atomic()
	//
	const Ring& next() const throw();
	Ring& next() throw();
	
	
	//
	// Return the element before this.
	// 
	// Preconditions:
	// 
	//    None.
	//
	// Postconditions:
	//
	//    returns *this || !atomic()
	//
	const Ring& prev() const throw();
	Ring& prev() throw();
	
	
	//
	// Merges this into the
	// specified ring, as defined below. (Note that
	// in the simple case where atomic(), the effect
	// is to insert this just before ring.)
	//
	// Preconditions
	// 
	//     None
	//
	// Postconditions:
	// 
	//     next()' = ring
	//     ring.prev()' = *this
	//     ring.prev().next()' = *this.next()
	//     next().prev()' = ring.prev()
	//
	void splice(const Ring& ring) const throw(); 
	
	
	//
	// Cut the specified section from the ring it
	// is in.
	//
	// Preconditions:
	//
	// Postconditions:
	//
	//    from.prev().next()' = to.next()
	//    to.next().prev()' = from.prev()
	//    from.prev()' = to
	//    to.next()' = from
	//
	void cut(const Ring& to) const throw();
	
	
	//
	// Just does cut(*this)
	//
	void cut() const throw();
	
	
	//
	// Swap this element with the one that follows
	// it.
	//
	// Postconditions:
	//
	//    prev()' == next()
	//    next()' == next().next()
	//
	void swapWithNext() const throw();
	
	
	//
	// Equality is defined such that no two distinct
	// links are equal.
	//
	// Postconditions:
	//
	//    returns zero || (this == &b)
	//
	int operator==(const Ring& b) const throw();
	int operator!=(const Ring& b) const throw();
	
	
	//
	// removes itself from the current
	// ring (if any).
	//
	// Preconditions
	// 
	//    None.
	//
	// Postconditions
	//
	//    atomic()
	// 
	~Ring();
	
	
	
    private:
	Ring<T>* m_p;
	Ring<T>* m_n;
	
	//
	// Work around for lack of mutable
	//
	Ring<T>*& p() const { return *(Ring<T>**)&m_p; };
	Ring<T>*& n() const { return *(Ring<T>**)&m_n; };
    };


    template<class T>
    inline int Ring<T>::atomic() const throw()
    {
	return n() == this;
    }
    
    
    template<class T>
    inline Ring<T>::Ring(T x):
	m_value(x),
	m_p(this),
	m_n(this)
    {
    }
    
    
    template<class T>
    Ring<T>::Ring(const Ring& x):
	m_value(x.m_value),
	m_p(this),
	m_n(this)
    {
    }
    
    
    template<class T>
    const Ring<T>& Ring<T>::operator=(const Ring& x)
    {
	if (this != &x)
	{
	    m_value = x.m_value;
	}
	return *this;
    }
    
    
    template<class T>
    const Ring<T>& Ring<T>::operator=(const T& x)
    {
	m_value = x;
	return *this;
    }
    
    
    template<class T>
    inline const Ring<T>& Ring<T>::next() const throw()
    {
	return *m_n;
    }
    
    
    template<class T>
    inline Ring<T>& Ring<T>::next() throw()
    {
	return *m_n;
    }
    
    
    template<class T>
    inline const Ring<T>& Ring<T>::prev() const throw()
    {
	return *m_p;
    }
    
    
    template<class T>
    inline Ring<T>& Ring<T>::prev() throw()
    {
	return *m_p;
    }
    
    
    template<class T>
    inline void Ring<T>::splice(const Ring<T>& ring) const throw()
    {
	next().p() = (Ring<T>*)&ring.prev();
	ring.prev().n() = n();
	n() = (Ring<T>*)&ring;
	ring.p() = (Ring<T>*)this;
    }
    
    
    template<class T>
    inline void Ring<T>::cut(const Ring<T>& to) const throw()
    {
	const Ring<T>& from(*this);
	
	from.prev().n() = to.n();
	to.next().p() = from.p();
	
	from.p() = (Ring<T>*)&to;	    
	to.n() =   (Ring<T>*)&from;   
    }
    
    
    template<class T>
    inline void Ring<T>::cut() const throw()
    {
	cut(*this);
    }
    
    
    template<class T>
    inline void Ring<T>::swapWithNext() const throw()
    {
	const Ring<T>& t = *n();
	
	cut(*this);
	t.splice(*this);
    }
    
    
    template<class T>
    inline Ring<T>::~Ring()
    {
	cut(*this);
    }
    
    
    template<class T>
    inline int Ring<T>::operator==(const Ring& b) const throw()
    {
	return this == &b;
    }
    
    
    template<class T>
    inline int Ring<T>::operator!=(const Ring& b) const throw()
    {
	return this != &b;
    }
}


#endif
