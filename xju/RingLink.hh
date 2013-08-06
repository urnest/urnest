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
//    Class that represents a link (and a ring of links).
//
#ifndef _XJU_RINGLINK_HH_
#define _XJU_RINGLINK_HH_


namespace xju
{
    class RingLink
    {
    private:
	RingLink* p;
	RingLink* n;
	
    public:
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
	//
	RingLink() throw();
	
	
	//
	// Insert new ring into host ring (just after hostRing)
	// 
	// Preconditions:
	// 
	// Postconditions:
	//
	//    !atomic() && !hostRing->atomic()
	//    hostRing.next() == *this
	//    prev() == hostRing
	//
	RingLink(RingLink& hostRing) throw();
	
	
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
	RingLink& next() const throw();
	
	
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
	RingLink& prev() const throw();
	
	
	//
	// Merges this ring into the
	// specified host ring, as defined below. (Note that
	// in the simple case where atomic(), the effect
	// is to insert this link just after hostRing.)
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
	void splice(RingLink& ring) throw(); 
	
	
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
	void cut(RingLink& to) throw();
	
	
	//
	// Just does cut(*this)
	//
	void cut() throw();
	
	
	//
	// Swap this element with the one that follows
	// it.
	//
	// Postconditions:
	//
	//    prev()' == next()
	//    next()' == next().next()
	//
	void swapWithNext() throw();
	
	
	//
	// Equality is defined such that no two distinct
	// links are equal.
	//
	// Postconditions:
	//
	//    returns zero || (this == &b)
	//
	int operator==(const RingLink& b) const throw();
	int operator!=(const RingLink& b) const throw();
	
	
	//
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
	~RingLink() throw();
	
    private:
	//
	// Not implemented
	//
	RingLink& operator=(const RingLink&);
	
    };
    
    
    inline int RingLink::atomic() const throw()
    {
	return n == this;
    }
    
    
    inline RingLink::RingLink() throw():
	p(this),
	n(this)
    {
    }
    
    
    inline RingLink& RingLink::next() const throw()
    {
	return *n;
    }
    
    
    inline RingLink& RingLink::prev() const throw()
    {
	return *p;
    }
    
    
    inline void RingLink::splice(RingLink& ring) throw()
    {
	next().p = &ring.prev();
	ring.prev().n = n;
	n = &ring;
	ring.p = this;
    }
    
    
    inline RingLink::RingLink(RingLink& hostRing) throw():
	p(this),
	n(this)
    {
	splice(hostRing);
    }
    
    
    inline void RingLink::cut(RingLink& to) throw()
    {
	RingLink& from(*this);
	
	from.prev().n = to.n;
	to.next().p = from.p;
	
	from.p = &to;	    
	to.n =   &from;   
    }
    
    
    inline void RingLink::cut() throw()
    {
	cut(*this);
    }
    
    
    inline void RingLink::swapWithNext() throw()
    {
	RingLink& t = *n;
	
	cut(*this);
	t.splice(*this);
    }
    
    
    inline RingLink::~RingLink() throw()
    {
	cut();
    }
    
    
    inline int RingLink::operator==(const RingLink& b) const throw()
    {
	return this == &b;
    }
    
    
    inline int RingLink::operator!=(const RingLink& b) const throw()
    {
	return this != &b;
    }
}


#endif
