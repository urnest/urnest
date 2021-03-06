// Copyright (c) 1997 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    Assertion functions.
//
//    Notes:
//
//    [1] I prefer these to simple logical
//        assertion (e.g. assert_(a != b)), since on failure
//        they produce a core file containing the evaluated
//        values of a and b, which can otherwise be difficult
//        to deduce.
//
//    [2] name ends in underscore to avoid clash with macro
//        defined in standard header file <assert.h>
//
//    [3] we return bool not void so that we can use STL algorithms
//        like std::equal with the function.
//
#ifndef _DG_ASSERT_HH_
#define _DG_ASSERT_HH_


#include <stdlib.h>
#include <algorithm>
#include <utility>
#include <string>


namespace dg
{
    void assert_abort() throw();

    template<class A, class B>
    bool assert_equal(const A& a, const B& b)
    {
	if (!(a == b))
	{
	    assert_abort();
	}
	return true;
    }
    template<class A, class B>
    bool assert_not_equal(const A& a, const B& b)
    {
	if (!(a != b))
	{
	    assert_abort();
	}
	return true;
    }
    template<class Operator, class A>
    bool assert_(const A& a, const Operator& o) // [2]
    {
	if (!o(a))
	{
	    assert_abort();
	}
	return true;
    }
#if defined(__GNUC__) && (__GNUC__ < 3 || (__GNUC__==3 && __GNUC_MINOR__<=3))
    template<class Operator, class A, class B>
    bool assert_(const A& a, Operator o, const B& b) // [2]
    {
	if (!o(a, b))
	{
	    assert_abort();
	}
	return true;
    }
#else
    template<class Operator, class A, class B>
    bool assert_(const A& a, const Operator& o, const B& b) // [2]
    {
	if (!o(a, b))
	{
	    assert_abort();
	}
	return true;
    }
#endif
    
    //
    // Here we specialize to ease debugging (gdb tends not to print
    // whole strings - it truncates after a few characters - so we
    // figure out where the difference occurs, and leave the same
    // and different bits lying around in variables so we can
    // see them in the debugger.
    //
    template<>
    inline bool assert_equal(const std::string& a, const std::string& b)
    {
	std::string commonPrefix;		  // placed so gdb to works reliably
	
	const std::string::size_type size_a(a.size());
	const std::string::size_type size_b(b.size());
	
	// to avoid unused warnings
	assert_equal(sizeof(size_a), sizeof(size_b));
	
	if (a.size() < b.size())
	{
	    const std::string trunc(b.substr(0, a.size()));
	    std::pair<std::string::const_iterator, std::string::const_iterator> r =
		mismatch(a.begin(), a.end(), trunc.begin());
	    commonPrefix = std::string(a.begin(), r.first);
	    assert_abort();
	}
	else if (b.size() < a.size())
	{
	    const std::string trunc(a.substr(0, a.size()));
	    std::pair<std::string::const_iterator, std::string::const_iterator> r =
		mismatch(b.begin(), b.end(), trunc.begin());
	    commonPrefix = std::string(b.begin(), r.first);
	    assert_abort();
	}
	else
	{
	    std::pair<std::string::const_iterator, std::string::const_iterator> r =
		mismatch(a.begin(), a.end(), b.begin());
	    if (r.first != a.end())
	    {
		commonPrefix = std::string(a.begin(), r.first);
		assert_abort();
	    }
	}
	return true;
    }
    
    //
    // aCC A.01.21 has trouble deriving the type T from the type
    // template "const T&" when given the type specifier "const char*&"
    // (in this case the compiler should recognise that T is char*, but
    // aCC gives a compile error).
    //
    // Using a typedef gets aCC A.01.21 over the line.
    //
    typedef char* assert_char_pointer;
    
    template<>
    inline bool assert_equal(const assert_char_pointer& a, const std::string& b)
    {
	return assert_equal(std::string(a), b);
    }
    template<>
    inline bool assert_equal(const std::string& a, const assert_char_pointer& b)
    {
	return assert_equal(a, std::string(b));
    }
};


#endif
