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
//    Functional programming tools.
//
//  RATIONALE
//
//    std c++ doesn't really cut it; boost is much better but
//    bloated
//    REVISIT: get rid of now C++ has lambda functions (perhaps
//    First and Second are still useful for working with maps?)
//
#ifndef _XJU_FUNCTIONAL_H_
#define _XJU_FUNCTIONAL_H_

#include <functional>
#include <utility>

namespace xju
{
    namespace functional
    {
        class First
        {
        public:
            template<class T, class U>
            T const& operator()(std::pair<T, U> const& x) const throw()
            {
                return x.first;
            }
            template<class T, class U>
            T& operator()(std::pair<T, U>& x) const throw()
            {
                return x.first;
            }
        };
        class Second
        {
        public:
            template<class T, class U>
            U const& operator()(std::pair<T, U> const& x) const throw()
            {
                return x.second;
            }
            template<class T, class U>
            U& operator()(std::pair<T, U>& x) const throw()
            {
                return x.second;
            }
        };
        extern First first;
        extern Second second;
    }
}

#endif
