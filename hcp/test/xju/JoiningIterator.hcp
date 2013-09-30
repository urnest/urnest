//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 2007 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    An ostream iterator that puts a delimiter
//    between items. (Rather than after every item, which is
//    what std::ostream_iterator does.)
//
//    Example:
//
//       const char x[] = { "A", "B", "C" }
//
//       std::ostringstream result;
//
//       copy(x,
//            &x[3],
//            xju::JoiningIterator<string, string>(result, ", "));
//
//       xju::assert_equal(string(result), "A, B, C");
//
// ============================================================
#ifndef _XJU_JOININGITERATOR_HH_
#define _XJU_JOININGITERATOR_HH_


#include <iterator>
#include <iostream>


namespace xju
{
	
    template<class T, class Delimiter> 
    class JoiningIterator : public std::output_iterator_tag {
    public:
        typedef T value_type;
        typedef T& reference;
        typedef T* pointer;
        typedef size_t difference_type;
        typedef std::output_iterator_tag iterator_category;
        
	//
	// Preconditions:
	//
	//    lifetime(s) > lifetime(this)
	//
	// Postconditions:
	//
	//    s' == s
	//    first()'
	//
	// Exceptions:
	//
	//    (exceptions of Delimiter(const Delimiter&))
	//
	JoiningIterator<T, Delimiter>(std::ostream& s, 
				      const Delimiter& delimiter):
	    _s(&s),
	    _delimiter(delimiter),
	    _first(true) {
	}
	
	
	//
	// Insert value, appropriately delimited, into s
	//
	// Not reentrant.
	//
	// Preconditions:
	//
	//    none
	//
	// Postconditions:
	//
	//    first() || value inserted into s
	//    !first() || delimiter then value inserted into s
	//
	//    !first()'
	//
	//     result = *this
	//
	// Exceptions:
	//
	//    exceptions of operator<<(ostream&, const Delimiter&)
	//        post: first()' == first()
	//    
	//    exceptions of operator<<(ostream&, const T&)
	//        post: delimiter inserted || first()
	//              first()' == first()
	//
	JoiningIterator<T, Delimiter>& operator=(const T& value)
	{
	    if (!_first) {
		(*_s) << _delimiter;
	    }
	    (*_s) << value;
	    _first = false;
	    
	    return *this;
	}
	
	
	//
	// Have any values been inserted into s?
	//
	// Not reentrant.
	//
	// Preconditions:
	//
	//    none
	//
	// Postconditions:
	//
	//    result == (operator=(const T&) not previously called)
	//
	// Exceptions:
	//
	//    none
	//
	bool first() const throw() {
	    return _first;
	}
	
	
	JoiningIterator<T, Delimiter>& operator*()     { return *this; }
	JoiningIterator<T, Delimiter>& operator++()    { return *this; } 
	JoiningIterator<T, Delimiter>& operator++(int) { return *this; }
	
	
    private:
	std::ostream*  _s;
	Delimiter _delimiter;
	bool      _first;
    };
    
}

#endif
