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
//    A template list class that notifies clients when items
//    are added to it/removed from it.
//
#ifndef _XJU_NOTIFYINGLIST_HH_
#define _XJU_NOTIFYINGLIST_HH_


#include <utility>
#include <sys/types.h>
#include <list>
#include <xju/Event.hh>
#include <xju/EventP.hh>


namespace xju
{
    template<class T>
    class NotifyingList
    {
    private:
	struct ItemEvents
	{
	    Event m_removing;
	    Event m_newPredecessor;
            bool operator<(const ItemEvents& x) const throw()
            {
                // REVISIT: really item_type::operator< should be
                // redefined in terms of only T
                return false;
            }
	};
	typedef std::pair<T, ItemEvents> item_type;
	typedef std::list<item_type> C;
	
    public:
	typedef T         value_type;
	typedef T&        reference;
	typedef const T&  const_reference;
	typedef size_t    size_type;
	typedef ptrdiff_t difference_type;

	class const_iterator;
	
	class iterator
	{
	public:
	    typedef T value_type;
	    typedef T&        reference;
	    typedef const T&  const_reference;
	    typedef T* pointer;
	    typedef ptrdiff_t difference_type;
	    typedef size_t    size_type;
	    typedef std::bidirectional_iterator_tag iterator_category;

	    iterator();
	    iterator(const iterator& b);
	    
	    
	    bool      operator==(const iterator& b) const;
	    bool      operator!=(const iterator& b) const;
	    
	    reference operator*() const;
	    iterator& operator++();
	    iterator  operator++(int);
	    iterator& operator--();
	    iterator  operator--(int);

	    //operator const_iterator() const throw();
	    
	private:
	    typename std::list<item_type>::iterator m_i;
	    
	    iterator(const typename std::list<item_type>::iterator& b);
	    
	    friend class NotifyingList;
	    friend class const_iterator;
	};
	
	class const_iterator
	{
	public:
	    typedef const T value_type;
	    typedef const T* pointer;
	    typedef T&        reference;
	    typedef const T&  const_reference;
	    typedef ptrdiff_t difference_type;
	    typedef size_t    size_type;
	    typedef std::bidirectional_iterator_tag iterator_category;
	    
	    const_iterator();
	    const_iterator(const const_iterator& b);
	    const_iterator(const iterator& b);
	    
	    bool            operator==(const const_iterator& b) const;
	    bool            operator!=(const const_iterator& b) const;
	    const_reference operator*() const;
	    const_iterator& operator++();
	    const_iterator  operator++(int);
	    const_iterator& operator--();
	    const_iterator  operator--(int);
	    
	private:
	    typename std::list<item_type>::iterator m_i;
	    
	    const_iterator(const typename std::list<item_type>::iterator& b);
	    
	    
	    friend class NotifyingList;
	    friend class iterator;
	};
	
	
    public:

	//
	// Return a reference to the event that
	// will be triggered if the item referenced by
	// the specified iterator is removed from the list.
	//
	// pre: *this != end()
	//
	const Event& removing(const_iterator) const throw();
	
	//
	// Return a reference to the event that
	// will be triggered when an item is inserted
	// before the specified iterator.
	//
	const Event& newPredecessor(const_iterator) const throw();
	
	
	bool           empty()    const;
	size_type      size()     const;
	size_type      max_size() const;
	iterator       begin();
	iterator       end();
	const_iterator begin() const;
	const_iterator end()   const;
	reference       front();
	reference       back();
	const_reference front() const;
	const_reference back() const;
	
	iterator insert(const_iterator position, const T& x);
	iterator insert(const_iterator position, size_type n, const T& x);
	iterator insert(const_iterator position,
			const_iterator first,
			const_iterator last);
	
	void erase(const_iterator position);
	void erase(const_iterator first, const_iterator last);
	
	iterator mute(const_iterator i);
	
	~NotifyingList();

	bool operator<(const NotifyingList<T>& x) const throw();
        
    private:
	mutable C m_items;
	Event m_addedAtEnd;
    };
    
    template<class T>
    NotifyingList<T>::iterator::iterator()
	: m_i()
    {
    }
    template<class T>
    NotifyingList<T>::iterator::iterator(const iterator& b):
	m_i(b.m_i)
    {
    }
    template<class T>
    bool NotifyingList<T>::iterator::operator==(const iterator& b) const
    {
	return m_i == b.m_i;
    }
    template<class T>
    bool NotifyingList<T>::iterator::operator!=(const iterator& b) const
    {
	return m_i != b.m_i;
    }
    template<class T>
    typename NotifyingList<T>::reference
    NotifyingList<T>::iterator::operator*() const
    {
	return (*m_i).first;
    }
    template<class T>
    typename NotifyingList<T>::iterator&
    NotifyingList<T>::iterator::operator++()
    {
	++m_i;
	return *this;
    }
    template<class T>
    typename NotifyingList<T>::iterator
    NotifyingList<T>::iterator::operator++(int)
    {
	iterator t(*this);
	++m_i;
	return t;
    }
    template<class T>
    typename NotifyingList<T>::iterator&
    NotifyingList<T>::iterator::operator--()
    {
	--m_i;
	return *this;
    }
//     template<class T>
//     NotifyingList<T>::iterator::operator 
//     typename NotifyingList<T>::const_iterator()	const throw()
//     {
// 	return typename NotifyingList<T>::const_iterator(m_i);
//     }
    template<class T>
    typename NotifyingList<T>::iterator
    NotifyingList<T>::iterator::operator--(int)
    {
	iterator t(*this);
	--m_i;
	return t;
    }
    
    
    template<class T>
    const Event& NotifyingList<T>::removing(
	NotifyingList<T>::const_iterator i) const throw()
    {
	return (*(i.m_i)).second.m_removing;
    }
    
    template<class T>
    const Event& NotifyingList<T>::newPredecessor(
	NotifyingList<T>::const_iterator i) const throw()
    {
	if (i == end())
	{
	    return m_addedAtEnd;
	}
	else
	{
	    return (*(i.m_i)).second.m_newPredecessor;
	}
    }
    
    template<class T>
    NotifyingList<T>::iterator::iterator(
	const typename std::list<item_type>::iterator& b)
	: m_i(b)
    {
    }
    
    
    template<class T>
    NotifyingList<T>::const_iterator::const_iterator(
	const typename std::list<item_type>::iterator& b)
	: m_i(b)
    {
    }
    template<class T>
    NotifyingList<T>::const_iterator::const_iterator()
	: m_i()
    {
    }
    template<class T>
    NotifyingList<T>::const_iterator::const_iterator(const const_iterator& b)
	: m_i(b.m_i)
    {
    }
    template<class T>
    NotifyingList<T>::const_iterator::const_iterator(const iterator& b)
	: m_i(b.m_i)
    {
    }
    template<class T>
    bool NotifyingList<T>::const_iterator::operator==(const const_iterator& b) const
    {
	return m_i == b.m_i;
    }
    template<class T>
    bool NotifyingList<T>::const_iterator::operator!=(const const_iterator& b) const
    {
	return m_i != b.m_i;
    }
    template<class T>
    typename NotifyingList<T>::const_reference
    NotifyingList<T>::const_iterator::operator*() const
    {
	return (*m_i).first;
    }
    template<class T>
    typename NotifyingList<T>::const_iterator&
    NotifyingList<T>::const_iterator::operator++()
    {
	++m_i;
	return *this;
    }
    template<class T>
    typename NotifyingList<T>::const_iterator
    NotifyingList<T>::const_iterator::operator++(int)
    {
	const_iterator t(*this);
	++m_i;
	return t;
    }
    template<class T>
    typename NotifyingList<T>::const_iterator&
    NotifyingList<T>::const_iterator::operator--()
    {
	--m_i;
	return *this;
    }
    template<class T>
    typename NotifyingList<T>::const_iterator
    NotifyingList<T>::const_iterator::operator--(int)
    {
	const_iterator t(*this);
	--m_i;
	return t;
    }
    
    
    template<class T>
    bool NotifyingList<T>::empty() const
    {
	return m_items.empty();
    }
    
    
    template<class T>
    typename NotifyingList<T>::size_type NotifyingList<T>::size() const
    {
	return m_items.size();
    }
    
    
    template<class T>
    typename NotifyingList<T>::size_type NotifyingList<T>::max_size() const
    {
	return m_items.max_size();
    }
    
    
    template<class T>
    typename NotifyingList<T>::iterator NotifyingList<T>::begin()
    {
	return iterator(m_items.begin());
    }
    
    
    template<class T>
    typename NotifyingList<T>::iterator NotifyingList<T>::end()
    {
	return iterator(m_items.end());
    }
    
    
    template<class T>
    typename NotifyingList<T>::const_iterator NotifyingList<T>::begin() const
    {
	return const_iterator(m_items.begin());
    }
    
    
    template<class T>
    typename NotifyingList<T>::const_iterator NotifyingList<T>::end() const
    {
	return const_iterator(m_items.end());
    }
    
    
    template<class T>
    typename NotifyingList<T>::reference NotifyingList<T>::front()
    {
	return *begin();
    }
    
    
    template<class T>
    typename NotifyingList<T>::const_reference NotifyingList<T>::front() const
    {
	return *begin();
    }
    
    
    template<class T>
    typename NotifyingList<T>::reference NotifyingList<T>::back()
    {
	iterator i = end();
	return *(--i);
    }
    
    
    template<class T>
    typename NotifyingList<T>::const_reference NotifyingList<T>::back() const
    {
	const_iterator i = end();
	return *(--i);
    }
    
    
    template<class T>
    typename NotifyingList<T>::iterator NotifyingList<T>::insert(
	const_iterator position, const T& x)
    {
	iterator i(m_items.insert(position.m_i,
				  std::make_pair(x, ItemEvents())));
	if (position == end())
	{
	    m_addedAtEnd();
	}
	else
	{
	    (*(position.m_i)).second.m_newPredecessor();
	}
	return i;
    }
    
    
    template<class T>
    typename NotifyingList<T>::iterator NotifyingList<T>::insert(
	const_iterator position, size_type n, const T& x)
    {
	while(n)
	{
	    insert(position, x);
	    ++n;
	}
    }
    
    
    template<class T>
    typename NotifyingList<T>::iterator NotifyingList<T>::insert(
	const_iterator position, const_iterator first, const_iterator last)
    {
	while(first != last)
	{
	    insert(position, *first);
	    ++first;
	}
    }
    
    
    template<class T>
    void NotifyingList<T>::erase(const_iterator position)
    {
	(*(position.m_i)).second.m_removing();
	m_items.erase(position.m_i);
    }
    
    
    template<class T>
    void NotifyingList<T>::erase(const_iterator first, const_iterator last)
    {
	while(first != last)
	{
            const_iterator i(first);
            ++first;
	    erase(i);
	}
    }

    template<class T>
    typename NotifyingList<T>::iterator NotifyingList<T>::mute(
	typename NotifyingList<T>::const_iterator i)
    {
	return iterator(i.m_i);
    }

    template<class T>
    bool NotifyingList<T>::operator<(const NotifyingList<T>& x) const throw()
    {
        return m_items < x.m_items;
    }
    
    template<class T>
    NotifyingList<T>::~NotifyingList()
    {
	while(!m_items.empty())
	{
	    m_items.erase(m_items.begin());
	}
    }
}

#endif

