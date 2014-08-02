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
//    REVISIT: is newer C++ 11 better
//
#ifndef _XJU_FUNCTIONAL_H_
#define _XJU_FUNCTIONAL_H_

#include <functional>
#include <utility>
#include "bits/method1.hh"
#include "bits/voidmethod1.hh"
#include "bits/constmethod1.hh"
#include "bits/voidconstmethod1.hh"
#include "bits/method2.hh"
#include "bits/voidmethod2.hh"
#include "bits/constmethod2.hh"
#include "bits/voidconstmethod2.hh"

namespace xju
{
    namespace functional
    {
        //
        // method - unary function from member function
        // deref - adapt function object to allow call with pointer argument
        //
        // see test-functional.cc for examples
        //
        // REVISIT: allow deref use with binary functions, providing all
        //          combinations of dereferencing?
        //          add select1st, select2nd using member pointers to
        //          derive result type

        template<class T, class R>
        ConstMethod<T, R> method(R (T::*method)() const) throw()
        {
            return ConstMethod<T, R>(method);
        }
        template<class T>
        VoidConstMethod<T> method(void (T::*method)() const) throw()
        {
            return VoidConstMethod<T>(method);
        }
        template<class T, class R>
        Method<T, R> method(R (T::*method)()) throw()
        {
            return Method<T, R>(method);
        }
        template<class T>
        VoidMethod<T> method(void (T::*method)()) throw()
        {
            return VoidMethod<T>(method);
        }
        template<class T, class R, class P>
        ConstMethod2<T, R, P> method(R (T::*method)(P) const) throw()
        {
            return ConstMethod2<T, R, P>(method);
        }
        template<class T, class P>
        VoidConstMethod2<T, P> method(void (T::*method)(P) const) throw()
        {
            return VoidConstMethod2<T, P>(method);
        }
        template<class T, class R, class P>
        Method2<T, R, P> method(R (T::*method)(P)) throw()
        {
            return Method2<T, R, P>(method);
        }
        template<class T, class P>
        VoidMethod2<T, P> method(void (T::*method)(P)) throw()
        {
            return VoidMethod2<T, P>(method);
        }

        template<class F>
        class Deref : public std::unary_function<typename F::argument_type*, typename F::result_type>
        {
        public:
            Deref(F f) throw():
                m_f(f)
            {
            }
            template<class X>
            typename F::result_type operator()(X x) const
            {
                return m_f(*x);
            }
        private:
            F m_f;
        };

        template<class F>
        Deref<F> deref(F f) throw()
        {
            return Deref<F>(f);
        }

        template<class A, class R>
        Deref<std::unary_function<A*, R> > deref(
            R (*f)(A)) throw()
        {
            return deref(std::ptr_fun(f));
        }

        class First
        {
        public:
            template<class T, class U>
            T& operator()(std::pair<T, U> const& x) throw()
            {
                return x.first;
            }
        };
        class Second
        {
        public:
            template<class T, class U>
            T& operator()(std::pair<T, U> const& x) throw()
            {
                return x.second;
            }
        };
        extern First first;
        extern Second second;
    }
}

#endif
