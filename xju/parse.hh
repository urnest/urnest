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
//    string parsing utilities
//
#ifndef _XJU_PARSE_HH_
#define _XJU_PARSE_HH_

#include <utility>
#include <string>
#include <xju/Exception.hh>
#include <xju/format.hh>
#include <sstream>
#include <algorithm>

namespace xju
{
    namespace parse
    {
        template<class T>
        class PV : public std::pair<T, std::string>
        {
        public:
            template<class U>
            PV(U u) : std::pair<T, std::string>(u)
            {
            }
            template<class U, class V>
            PV(U u, V v) : std::pair<T, std::string>(u, v)
            {
            }
        };
        
        inline std::string skipWhite(const std::string& x)
        {
            std::string::const_iterator i;
            for(i = x.begin();
                i != x.end() && isspace(*i);
                ++i);
            return std::string(i, x.end());
        }
        inline PV<std::string> parseToChar(const std::string& x,
                                           const char c) throw(xju::Exception)
        {
            try
            {
                std::string::const_iterator i(std::find(x.begin(),
                                                        x.end(),
                                                        c));
                if (i == x.end())
                {
                    std::ostringstream s;
                    s << "no '" << c << "' found";
                    throw xju::Exception(s, XJU_TRACED);
                }
                return PV<std::string>(std::string(x.begin(), i),
                                       std::string(i, x.end()));
            }
            catch(xju::Exception& e)
            {
                std::ostringstream s;
                s << "parse to first occurrance of '" << c << "' in "
                  << xju::format::quote(s.str());
                e.addContext(s, XJU_TRACED);
                throw;
            }
        }

        // RewindableIterator:
        //  - RewindableIterator's copy constructor must not throw.
        //  - all functions are exception safe for exceptions thrown
        //    by RewindableIterator.operator++()
        //  - to use RewindableIterator whose operator++() throws other
        //    than xju::Exception, install a pass-through unexpected handler
        //    across calls to below functions.
        //  - bool RewindableIterator::atEnd() const indicates at end
        //  - throw of xju::Exception indicates malformed or read failure
        //    (i.e. iterator dereference/advance exception)
        //

        // wrap a standard iterator to make it
        // conform to RewindableIterator definition above
        template<class iterator>
        class IteratorAdaptor
        {
        public:
            IteratorAdaptor(iterator x, iterator end) throw():
                line_(1),
                column_(1),
                x_(x),
                end_(end)
            {
            }
            typename std::iterator_traits<iterator>::reference const operator*() const
                throw(xju::Exception);
            
            IteratorAdaptor operator++(int) throw()
            {
                IteratorAdaptor result(*this);
                operator++();
                return result;
            }
            IteratorAdaptor& operator++() throw(xju::Exception)
            {
                if (atEnd())
                {
                    std::ostringstream s;
                    s << "end of input at line " 
                      << line_ << " column " << column_;
                    throw xju::Exception(s, XJU_TRACED);
                }
                ++column_;
                if (*x_ == '\n')
                {
                    ++line_;
                    column_=1;
                }
                ++x_;
                return *this;
            }
            bool atEnd() const throw()
            {
                return x_ == end_;
            }
            friend std::ostream& operator<<(std::ostream& s, 
                                            IteratorAdaptor const& x) throw()
            {
                return s << "line " << x.line_ << " column " << x.column_;
            }
            
            unsigned int line_;
            unsigned int column_;
            iterator x_;
            iterator end_;

            friend bool operator==(IteratorAdaptor const& x,
                                   IteratorAdaptor const& y) throw()
            {
                return x.end_ == y.end_ &&
                    x.x_ == y.x_;
            }
            friend bool operator!=(IteratorAdaptor const& x,
                                   IteratorAdaptor const& y) throw()
            {
                return !(x==y);
            }
            friend bool operator<(IteratorAdaptor const& x,
                                  IteratorAdaptor const& y) throw()
            {
                if (x.x_<y.x_) return true;
                if (y.x_<x.x_) return false;
                if (x.end_<y.end_) return true;
                if (y.end_<x.end_) return false;
                return false;
            }
            friend bool operator>(IteratorAdaptor const& x,
                                  IteratorAdaptor const& y) throw()
            {
                return y<x;
            }
        };

        template<class iterator>
        typename std::iterator_traits<iterator>::reference const 
        IteratorAdaptor<iterator>::operator*() const
            throw(xju::Exception)
        {
            if (atEnd())
            {
                std::ostringstream s;
                s << "end of input at line " << line_ << " column " << column_;
                throw xju::Exception(s, XJU_TRACED);
            }
            return (*x_);
        }

    }
}

#endif
