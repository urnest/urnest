//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 2004
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef _XJU_OPTIONAL_HH_
#define _XJU_OPTIONAL_HH_

#include <memory>

namespace xju
{
    template<class T>
    class Optional
    {
    public:
        Optional() throw()
        {
        }
        Optional(T x):
            m_x(new T(x))
        {
        }
        Optional(const Optional<T>& x)
        {
            if (x.valid()) {
                m_x = std::auto_ptr<T>(new T(*x.m_x));
            }
        }
        bool valid() const throw()
        {
            return m_x.get() != 0;
        }
        T& value() throw()
        {
            return *m_x;
        }
        const T& value() const throw()
        {
            return *m_x;
        }
        const T* get() const throw()
        {
            return m_x.get();
        }
        T* get() throw()
        {
            return m_x.get();
        }
        const T& operator*() const throw()
        {
            return *m_x;
        }
        T& operator*() throw()
        {
            return *m_x;
        }
        
        void clear() throw()
        {
            m_x.reset();
        }
        void reset() throw()
        {
            return m_x.reset();
        }
        Optional& operator=(const T& x)
        {
            if (!valid())
            {
                m_x = std::auto_ptr<T>(new T(x));
            }
            else 
            {
                (*m_x) = x;
            }
            return *this;
        }
        Optional& operator=(const Optional<T>& x)
        {
            if (this != &x)
            {
                if (!x.valid())
                {
                    clear();
                }
                else
                {
                    (*this) = x.value();
                }
            }
        }
        
    private:
        std::auto_ptr<T> m_x;
    };
    template<class T>
    bool operator<(const Optional<T>& x, const Optional<T>& y) 
    {
        if (!x.valid())
        {
            return y.valid();
        }
        return y.valid() && (x.value() < y.value());
    }
    template<class T>
    bool operator>(const Optional<T>& x, const Optional<T>& y)
    {
        return y < x;
    }
    template<class T>
    bool operator!=(const Optional<T>& x, const Optional<T>& y)
    {
        return x < y || x > y;
    }
    template<class T>
    bool operator==(const Optional<T>& x, const Optional<T>& y)
    {
        return !(x != y);
    }
    template<class T>
    bool operator>=(const Optional<T>& x, const Optional<T>& y)
    {
        return !(x < y);
    }
    template<class T>
    bool operator<=(const Optional<T>& x, const Optional<T>& y)
    {
        return !(x > y);
    }
}

#endif
