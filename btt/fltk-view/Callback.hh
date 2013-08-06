//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 2007
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    Callback to a method
//
#ifndef _BTT_VIEW_CALLBACK_HH_
#define _BTT_VIEW_CALLBACK_HH_

#include <fltk/Widget.h>
#include <boost/noncopyable.hpp>
#include <xju/Exception.hh>

namespace btt
{
    namespace view
    {
        void Callback_handle(xju::Exception const& e) throw();
        
        template<class T>
        class Callback : boost::noncopyable
        {
        public:
            fltk::Widget& widget_;
            T& target_;
            void (T::*method_)();

            Callback(fltk::Widget& widget,
                     T& target,
                     void (T::*method)()) throw():
                widget_(widget),
                target_(target),
                method_(method)
            {
                widget.callback(&Callback<T>::callback, this);
            }
            ~Callback() throw()
            {
                widget_.callback(0, (void*)0);
            }
            void trigger() throw()
            {
                try
                {
                    (target_.*method_)();
                }
                catch(xju::Exception const& e)
                {
                    Callback_handle(e);
                }
            }
            
        private:
            static void callback(fltk::Widget*, void* x) throw()
            {
                Callback<T>& c = *(Callback<T>*)x;
                c.trigger();
            }
        };
    }
}

        
#endif
