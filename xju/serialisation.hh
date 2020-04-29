//     -*- mode: c++ ; c-file-style: "osse" ; -*-
//
// Copyright (c) 2003
// Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    Binary serialisation of arbitrary types.
//
#ifndef XJU_SERIALISATION_H_
#define XJU_SERIALISATION_H_

#include <algorithm>

namespace xju
{
    namespace serialisation
    {
        //
        // Is this CPU reverse-byte-order to that we assume?
        //
        extern const bool byteSwapped;
        
        // pre: T is a POD type
        template<class T>
        void swapBytes(T& x) throw();
        
        template<class T> 
        inline void toHostOrder(T& x) throw()
        {
            if (byteSwapped)
            {
                swapBytes(x);
            }
        }
        template<class T>
        void fromHostOrder(T& x) throw()
        {
            if (byteSwapped)
            {
                swapBytes(x);
            }
        }

        //
        // specialisation must have:
        //   static void insert(std::ostream& s, T x) throw(xju::Exception)
        //   static T extract(std::istream& s) throw(xju::Exception)
        //   static std::string typeName() const throw()
        //   typeName().size() <= USHORT_MAX
        //
        // these must do any necessary byte reordering (byte reordering
        // is only necessary where a type is not expressed in terms
        // of the C++ "basic" types).
        //
        template<class T> class traits;

        // The following types are implemented down below:
        template<> class traits<char>;
        template<> class traits<unsigned char>;
        template<> class traits<short>;
        template<> class traits<unsigned short>;
        template<> class traits<int>;
        template<> class traits<unsigned int>;
        template<> class traits<std::string>;
        // REVISIT: do float and double too

        class istream
        {
        public:
            // pre: lifetime(s) > lifetime(this)
            istream(std::istream& s) throw();
            template<class T>
            T extract() /*throw(xju::Exception)*/
            {
                try
                {
                    const std::string t(traits<std::string>::extract(m_x));
                    if (t != traits<T>::typeName())
                    {
                        std::ostringstream s;
                        s << "next object is not of type "
                          << traits<T>::typeName()
                          << ", but is actually of type "
                          << t;
                        throw xju::Exception(s, XJU_TRACED);
                    }
                }
                catch(xju::Exception& e)
                {
                    std::ostringstream s;
                    s << "extract next object from serialised data, assuming "
                      << "it is of type "
                      << traits<T>::typeName();
                    e.addContext(s, XJU_TRACED);
                    throw;
                }
            }
        private:
            std::istream& m_x;
        };
        class ostream
        {
        public:
            ostream(std::ostream& s) throw();
            template<class T>
            void insert(const T& x) /*throw(xju::Exception)*/
            {
                try
                {
                    traits<std::string>::insert(traits<T>::typeName());
                    traits<T>::insert(x);
                }
                catch(xju::Exception& e)
                {
                    std::ostringstream s;
                    s << "serialise object of type " << traits<T>::typeName();
                    e.addContext(s, XJU_TRACED);
                    throw;
                }
            }
        private:
            std::ostream& m_x;
        };

        template<class T, class T1>
        class S1
        {
        public:
            S1(const T1& (T::*a1)() const) throw():
                _a1(a1)
            {
            }
            static void insert(std::ostream& s, const T& x) /*throw(
                xju::Exception)*/
            {
                traits<T1>::insert(s, (x.*_a1)());
            }
            static T extract(std::istream& s) /*throw(
                xju::Exception)*/
            {
                const T1 a1(traits<T1>::extract(s));
                return T(a1);
            }
        private:
            const T1& (T::*_a1)() const;
        };
    }
}
// implementation
namespace xju
{
    namespace serialisation
    {
        template<class T>
        void swapBytes(T& x) throw()
        {
            // REVISIT: use new style cast, but which?
            unsigned char* p1((unsigned char*)&x);
            unsigned char* p2(p1+sizeof(T)-1);
            for(size_t i = 0; i != sizeof(T)/2; ++i, ++p1, --p2)
            {
                std::swap(*p1, *p2);
            }
        }

        // pre: T is a POD type
        template<class T>
        class Pod
        {
        public:
            static void insert(std::ostream& s, const T x) /*throw(
                xju::Exception)*/
            {
                T y(x);
                fromHostOrder(y);
                const char* begin((char*)&y);
                const char* end(begin+sizeof(x));
                for(size_t i(0); i != end-begin; ++i)
                {
                    s.put(begin[i]);
                    if (!s)
                    {
                        std::ostringstream s;
                        s << "write " << xju::format::ord(i) << "byte";
                        throw xju::Exception(s, XJU_TRACED);
                    }
                }
            }
            static T extract(std::istream& s) /*throw(xju::Exception)*/
            {
                T result;
                char* p((char*)&y);
                for(size_t i(0); i != sizeof(T); ++i)
                {
                    char c;
                    s.get(c);
                    if (!s)
                    {
                        std::ostringstream s;
                        s << "read " << i << " byte";
                        throw xju::Exception(s, XJU_TRACED);
                    }
                    *p++ = c;
                }
                toHostOrder(result);
                return result;
            }
        };

        template<>
        class traits<std::string>
        {
            static void insert(std::ostream& s, const std::string& x) /*throw(
                xju::Exception)*/
            {
                try
                {
                    traits<unsigned long>::insert(s, x.size());
                    std::string::const_iterator i;
                    for(i = x.begin(); i != x.end(); ++i)
                    {
                        s.put(*i);
                        if (!s)
                        {
                            std::ostringstream s;
                            s << "std::ostream::put() failed putting "
                              << xju::format::ord(i-x.begin())
                              << " character ("
                              << xju::format::char_(*i);
                            throw xju::Exception(s, XJU_TRACED);
                        }
                    }
                }
                catch(xju::Exception& e)
                {
                    std::ostringstream s;
                    s << "serialise string "
                      << xju::format::quote(x);
                    e.addContext(s, XJU_TRACED);
                    throw;
                }
            }
            static std::string extract(std::istream& s) /*throw(
                xju::Exception)*/
            {
                const unsigned long l(traits<unsigned long>::extract(s));
                std::ostringstream result;
                for(unsigned long i(0); i != l; ++i)
                {
                    char c;
                    s.get(c);
                    if (!s)
                    {
                        std::ostringstream f;
                        f << "extract " << xju::format::ord(i)
                          << " character of string of length "
                          << l
                          << ", having already extracted "
                          << xju::format::quote(result.str());
                        throw xju::Exception(f, XJU_TRACED);
                    }
                    result.put(c);
                }
                return result.str();
            }
        };
    }
}

#define XJU_SERIALISATION_TRAITS_POD(P)                                 \
template<>                                                              \
class xju::serialisation::traits<P> : public xju::serialisation::Pod<P> \
{                                                                       \
    static std::string typeName() throw()                               \
    {                                                                   \
        return ##P;                                                     \
    }                                                                   \
}                                                                       \
template<>                                                              \
class xju::serialisation::traits<unsigned P> :                          \
  public xju::serialisation::Pod<unsigned P>                            \
{                                                                       \
    static std::string typeName() throw()                               \
    {                                                                   \
        return "unsigned" ##P;                                          \
    }                                                                   \
}
XJU_SERIALISATION_TRAITS_POD(char);
XJU_SERIALISATION_TRAITS_POD(short);
XJU_SERIALISATION_TRAITS_POD(int);

#define XJU_SERIALISATION_TRAITS_S1(T, a1)
template<>
class xju::serialisation::traits<T>
{
    static std::string typeName() throw()
    {
        return ##T;
    }
    static void insert(std::ostream& s, const T& x) /*throw(
        xju::Exception)*/
    {
        try
        {
            xju::serialisation::makeS(&T::a1).insert(s, x);
        }
        catch(xju::Exception& e)
        {
            std::ostringstream s;
            s << "serialise object of type " ##T;
            e.addContext(s, XJU_TRACED);
            throw;
        }
    }
    static T extract(std::istream& s) /*throw(
        xju::Exception)*/
    {
        try
        {
            xju::serialisation::makeS(&T::a1).extract(s);
        }
        catch(xju::Exception& e)
        {
            std::ostringstream s;
            s << "extract next object, assuming it is of type " ##T;
            e.addContext(s, XJU_TRACED);
            throw;
        }
    }
};

#endif
