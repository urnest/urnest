// -*- mode: c++; c-file-style: "osse"; -*-
//
// From code Copyright (c) 1997 Trevor Taylor
// Copyright (c) 2007 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    Pointer class that keeps track of references to an object
//    on the free store, deleting it when no more references exist.
//    
//    (See also ConstShared)
//
#ifndef _XJU_SHARED_HH_
#define _XJU_SHARED_HH_


#include <xju/RingLink.hh>
#include <memory>


namespace xju
{
    template<class T>
    class ConstShared;
    
    template<class T>
    class Shared;
    
    template<class V, class U>
    void constructShared(V& x, U const& source) throw()
    {
        if (source.m_value)
        {
            x.link().splice(source.link());
            x.m_value = source.m_value;
        }
    }
    
    template <class T>
    class Shared
    {
    public:
	typedef T value_type;
	
	
	//
	// Postconditions:
	//
	//    !valid()'
	//    count()' = 0
	//
	Shared() throw();
	
	
	//
	// Postconditions:
	//
	//    valid()'
	//    value()' == x
	//    count()' = 1
	//
	Shared(T* x) throw();
	
	template<class U>
	Shared(std::auto_ptr<U> x) throw();
	
	
	//
	// Reference the same value as x
	// 
	// Postconditions:
	//
	//    valid()' == x.valid()
	//    !valid()' || (value()' == x.value())
	//    !valid()' || (count() = x.count() + 1)
	//    !valid()' || (x.count()' = x.count() + 1)
	//
        template<class U>
	Shared(const Shared<U>& x) throw();
	
	
	//
	// Take a reference out on the object referenced by another
	// Shared<T> (if any).  Any previously referenced object
	// is "dereferenced".  If we were the last one referencing
	// the previous object, it is deleted (using delete)
	//
	// Postconditions:
	//
	//    (!valid() || ((value() deleted) || (count() > 1))
	//    valid()' == x.valid()
	//    !valid()' || (value()' == x.value())
	//    !valid()' || (count() = x.count() + 1)
	//    !valid()' || (x.count()' = x.count() + 1)
	//
	const Shared<T>& operator=(const Shared<T>& x) throw();
	
	
	//
	// Take a reference out on the specified object.  Any previously 
	// referenced object is "dereferenced".  If we were the last one 
	// referencing the previous object, it is deleted (using delete)
	//
	// Postconditions:
	//
	//    (!valid() || ((value() deleted) || others still have refs to value())
	//    valid()'
	//    value()' = x
	//    count()' = 1
	//
	const Shared<T>& operator=(T* x) throw();
	
	
	//
	// Report whether we reference any object
	//
	int valid() const throw();
	
	
	//
	// Return the object referenced
	//
	// Preconditions:
	//
	//    valid()
	//
	// Postconditions:
	//
	//    returned reference is valid while there are
	//    references to the object
	//
	operator const T&() const throw();
	operator       T&()       throw();
	
	T& value() const throw();
	
	T* operator->() const;
	
	T& operator*() const;
	
	//
	// Equality is defined as both Shared<T> referencing
	// the same object (or both !valid())
	//
	int operator==(const Shared<T>& x) const throw();
	int operator!=(const Shared<T>& x) const throw();
	
	
	//
	// Postconditions:
	//
	//    (count() > 1) || object deleted
	//
	~Shared() throw();
	
	
    protected:
	/* mutable */ RingLink m_link;
	
	T* m_value;
	
	
	//
	// For compilers without mutable
	//
	RingLink& link() const 
	{
	    return *(RingLink*)&m_link;
	}
	
	
	Shared(T* v, RingLink& l) throw():
	    m_value(v)
	{
	    if (m_value)
	    {
		link().splice(l);
	    }
	}
	
	
	//
	// Release the object pointed to.
	//
	// Preconditions:
	//
	//    None.
	//
	// Postconditions:
	//
	//    m_link.m_value' == 0
	//    m_link.atomic()'
	//    m_object deleted || !m_link.atomic()
	//
	void release();
	
	
	//
	// The count of references to the currently referenced
	// object (or zero if none)
	//
	// Not implemented, just here to make pre/post conditions
	// simpler.
	//
	int count() const;
	
	
	friend class ConstShared<T>;

        // construct x from source, assuming !x.valid()
        template<class V, class U>
        friend void constructShared(V& x, U const& source) throw();
    };
    
    
    template <class T>
    inline void Shared<T>::release()
    {
	if (link().atomic())
	{
	    T* tmp = m_value;
	    m_value = 0;
	    if (tmp)
	    {
		delete tmp;
	    }
	}
	else
	{
	    link().cut();
	    m_value = 0;
	}
    }
    
    
    template <class T>
    inline Shared<T>::Shared() throw():
	m_value(0)
    {
    }
    
    
    template <class T>
    inline Shared<T>::~Shared() throw()
    {
	release();
    }
    
    
    template <class T>
    inline Shared<T>::Shared(T* o) throw():
	m_value(o)
    {
    }
    
    
    template <class T> template<class U>
    inline Shared<T>::Shared(std::auto_ptr<U> x) throw():
	m_value(x.release())
    {
    }
    
    
    template <class T> template<class U>
    inline Shared<T>::Shared(const Shared<U>& source) throw():
	m_value(0)
    {
        constructShared(*this, source);
    }
    
    
    template <class T>
    inline const Shared<T>& Shared<T>::operator=(
	const Shared<T>& source) throw()
    {
	if (this != &source)
	{
	    release();
	    if (source.m_value)
	    {
		link().splice(source.link());
		m_value = source.m_value;
	    }
	}
	return *this;
    }
    
    
    template <class T>
    inline const Shared<T>& Shared<T>::operator=(T* o)
	throw()
    {
	release();
	m_value = o;
	return *this;
    }
    
    
    template <class T>
    inline Shared<T>::operator const T&() const
	throw()
    {
	return *m_value;
    }
    
    
    template <class T>
    inline Shared<T>::operator T&()
	throw()
    {
	return *m_value;
    }
    
    
    template <class T>
    inline T& Shared<T>::value() const
	throw()
    {
	return *m_value;
    }
    
    
    template<class T>
    inline T* Shared<T>::operator->() const
    {
	return m_value;
    }
    
    
    template<class T>
    inline T& Shared<T>::operator*() const
    {
	return *m_value;
    }
    
    
    template <class T>
    inline int Shared<T>::operator==(const Shared<T>& ref) const
	throw()
    {
	return (m_value == ref.m_value);
    }
    
    
    template <class T>
    inline int Shared<T>::operator!=(const Shared<T>& ref) const
	throw()
    {
	return (m_value != ref.m_value);
    }
    
    
    template <class T>
    inline int Shared<T>::valid() const throw()
    {
	return (m_value != 0);
    }
    
}

#endif
