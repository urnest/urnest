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
//    System call wrapper:
//
//       - turns failures into exceptions
//       - optionally retries if interrupted
//
//    (See examples below.)
//
#ifndef _XJU_SYSCALL_HH_
#define _XJU_SYSCALL_HH_


#include <xju/Exception.hh>
#include <xju/Traced.hh>
#include <errno.h>


namespace xju
{
    class SyscallInterrupted;
    class SyscallFailed;
    
    template<class R, class P1>
    class Syscall1;
    template<class R, class P1, class P2>
    class Syscall2;
    template<class R, class P1, class P2, class P3>
    class Syscall3;
    template<class R, class P1, class P2, class P3, class P4>
    class Syscall4;
    template<class R, class P1, class P2, class P3, class P4, class P5>
    class Syscall5;
    
    //
    // Descriptors for functions, each has a null-terminated string
    // name and a function pointer.
    //
    // Example:  SyscallF2("creat", ::creat)
    //           SyscallF3("lockf", ::lockf)
    //
    // Note: function name (e.g. "creat") must be valid for 
    // lifetime of object.
    //
    template<class R, class P1> struct SyscallF1;
    template<class R, class P1, class P2> struct SyscallF2;
    template<class R, class P1, class P2, class P3> struct SyscallF3;
    template<class R,
	class P1,
	class P2,
	class P3,
	class P4> struct SyscallF4;
    template<class R,
	class P1,
	class P2,
	class P3,
	class P4,
	class P5> struct SyscallF5;
    
    //
    // Call a specified system call with the specified parameters:
    //
    //    - by default, retry all calls that are interrupted
    //    - raise exception if system call fails (corresponding
    //      to a -1 return from the system call)
    //    - otherwise return the result of the system call.
    //
    // Examples:
    //
    //    1. Create a file, do not recover from any failure
    //
    //       #include <xju/fcntl.hh>
    //                     :
    //       int createFile(const std::string& f) throw(Exception)
    //       {
    //          try
    //          {
    //             return syscall(creat, XJU_TRACED)(
    //                f.c_str(),
    //                0777);
    //          }
    //          catch(Exception& e)
    //          {
    //             std::ostringstream s;
    //             s << "create file " << f;
    //             e.addContext(s, XJU_TRACED);
    //             throw;
    //          }
    // 
    //    2. Use lockf to try to lock a file, returning a bool indicating
    //       whether file was locked.
    //
    //       #include <xju/unistd.hh>
    //                     :
    //       bool lockFileIfPossible(const std::string& f,
    //                               size_t offset) throw(Exception)
    //       {
    //          try
    //          {
    //             try
    //             {
    //                syscall(lockf, XJU_TRACED)(
    //                   f.c_str(),
    //                   F_TLOCK,
    //                   offset);
    //                return true;
    //             }
    //             catch(const SyscallFailed& e)
    //             {
    //                if (e._errno == EACCES) // someone else has it
    //                {
    //                    return false;
    //                }
    //                throw; // other failure
    //             }
    //          }
    //          catch(Exception& e)
    //          {
    //             std::ostringstream s;
    //             s << "try to lock file " << f << " at offset " << offset";
    //             e.addContext(s, XJU_TRACED);
    //             throw;
    //          }
    //       }
    //
    template<class R, class P1>
    Syscall1<R, P1>
    syscall(const SyscallF1<R, P1>& f,
	    const Traced& location, 
	    bool retryIfInterrupted = true,
	    const R errorIndicator = -1) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed);
    
    template<class R, class P1>
    Syscall1<R, P1>
    syscall(const char* name,
	    R (*const f)(P1),
	    const Traced& location, 
	    bool retryIfInterrupted = true,
	    const R errorIndicator = -1) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed);
    
    template<class R, class P1, class P2>
    Syscall2<R, P1, P2>
    syscall(const SyscallF2<R, P1, P2>& f,
	    const Traced& location,
	    bool retryIfInterrupted = true,
	    const R errorIndicator = -1) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed);
    
    template<class R, class P1, class P2>
    Syscall2<R, P1, P2>
    syscall(const char* name,
	    R (*const f)(P1, P2),
	    const Traced& location,
	    bool retryIfInterrupted = true,
	    const R errorIndicator = -1) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed);
    
    template<class R, class P1, class P2, class P3>
    Syscall3<R, P1, P2, P3>
    syscall(const SyscallF3<R, P1, P2, P3>& f,
	    const Traced& location,
	    bool retryIfInterrupted = true) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed);
    
    template<class R, class P1, class P2, class P3>
    Syscall3<R, P1, P2, P3>
    syscall(const char* name,
	    R (*const f)(P1, P2, P3),
	    const Traced& location,
	    bool retryIfInterrupted = true) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed);

    template<class R, class P1, class P2, class P3, class P4>
    Syscall4<R, P1, P2, P3, P4>
    syscall(const SyscallF4<R, P1, P2, P3, P4>& f,
	    const Traced& location,
	    bool retryIfInterrupted = true) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed);
    
    template<class R, class P1, class P2, class P3, class P4>
    Syscall4<R, P1, P2, P3, P4>
    syscall(const char* name,
	    R (*const f)(P1, P2, P3, P4),
	    const Traced& location,
	    bool retryIfInterrupted = true) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed);

    template<class R, class P1, class P2, class P3, class P4, class P5>
    Syscall5<R, P1, P2, P3, P4, P5>
    syscall(const SyscallF5<R, P1, P2, P3, P4, P5>& f,
	    const Traced& location,
	    bool retryIfInterrupted = true) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed);
    
    template<class R, class P1, class P2, class P3, class P4, class P5>
    Syscall5<R, P1, P2, P3, P4, P5>
    syscall(const char* name,
	    R (*const f)(P1, P2, P3, P4, P5),
	    const Traced& location,
	    bool retryIfInterrupted = true) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed);
}


// implementation
namespace xju
{
    class SyscallInterrupted
    {
    };
    
    class SyscallFailed : public Exception
    {
    public:
	SyscallFailed(const std::string& name,
		      const int errno_,
		      const Traced& location) throw():
	    Exception(makeCause(name, errno_), location),
	    _errno(errno_)
	{
	}
	const int _errno;
    private:
	std::string makeCause(const std::string& name,
			      const int errno_) throw();
    };
    
    
    template<class R, class P1>
    struct SyscallF1
    {
	SyscallF1(const char* name, R (*const f)(P1)) throw():
	    _name(name),
	    _f(f)
	{
	}
	const char* _name;
	R (*const _f)(P1);
    };
    
    
    template<class R, class P1, class P2>
    struct SyscallF2
    {
	SyscallF2(const char* name, R (*const f)(P1, P2)) throw():
	    _name(name),
	    _f(f)
	{
	}
	const char* _name;
	R (*const _f)(P1, P2);
    };
    
    
    template<class R, class P1, class P2, class P3>
    struct SyscallF3
    {
	SyscallF3(const char* name, R (*const f)(P1, P2, P3)) throw():
	    _name(name),
	    _f(f)
	{
	}
	const char* _name;
	R (*const _f)(P1, P2, P3);
    };
    
    
    template<class R, class P1, class P2, class P3, class P4>
    struct SyscallF4
    {
	SyscallF4(const char* name, R (*const f)(P1, P2, P3, P4)) throw():
	    _name(name),
	    _f(f)
	{
	}
	const char* _name;
	R (*const _f)(P1, P2, P3, P4);
    };
    
    
    template<class R, class P1, class P2, class P3, class P4, class P5>
    struct SyscallF5
    {
	SyscallF5(const char* name, R (*const f)(P1, P2, P3, P4, P5)) throw():
	    _name(name),
	    _f(f)
	{
	}
	const char* _name;
	R (*const _f)(P1, P2, P3, P4, P5);
    };
    
    
    template<class R, class P1>
    class Syscall1
    {
    public:
	Syscall1(const SyscallF1<R, P1>& f,
		 const bool retryIfInterrupted,
		 const Traced& location,
		 const R errorIndicator = -1) throw():
	    _f(f),
	    _retryIfInterrupted(retryIfInterrupted),
	    _location(location),
	    _errorIndicator(errorIndicator)
	{
	}
	R operator()(P1 p1) const throw(
	    //
	    // pre: !retryIfInterrupted &&
	    //      signal interrupted the system call
	    //
	    SyscallInterrupted,
	    SyscallFailed);
    private:
	const SyscallF1<R, P1> _f;
	const bool _retryIfInterrupted;
	const Traced _location;
	const R _errorIndicator;
    };
    
    
    template<class R, class P1, class P2>
    class Syscall2
    {
    public:
	Syscall2(const SyscallF2<R, P1, P2>& f,
		 const bool retryIfInterrupted,
		 const Traced& location,
		 const R errorIndicator) throw():
	    _f(f),
	    _retryIfInterrupted(retryIfInterrupted),
	    _location(location),
	    _errorIndicator(errorIndicator)
	{
	}
	R operator()(P1 p1, P2 p2) const throw(
	    //
	    // pre: !retryIfInterrupted &&
	    //      signal interrupted the system call
	    //
	    SyscallInterrupted,
	    SyscallFailed);
    private:
	const SyscallF2<R, P1, P2> _f;
	const bool _retryIfInterrupted;
	const Traced _location;
	const R _errorIndicator;
    };
    
    
    template<class R, class P1, class P2, class P3>
    class Syscall3
    {
    public:
	Syscall3(const SyscallF3<R, P1, P2, P3>& f,
		 const bool retryIfInterrupted,
		 const Traced& location) throw():
	    _f(f),
	    _retryIfInterrupted(retryIfInterrupted),
	    _location(location)
	{
	}
	R operator()(P1 p1, P2 p2, P3 p3) const throw(
	    //
	    // pre: !retryIfInterrupted &&
	    //      signal interrupted the system call
	    //
	    SyscallInterrupted,
	    SyscallFailed);
    private:
	const SyscallF3<R, P1, P2, P3> _f;
	const bool _retryIfInterrupted;
	const Traced _location;
    };
    
    
    template<class R, class P1, class P2, class P3, class P4>
    class Syscall4
    {
    public:
	Syscall4(const SyscallF4<R, P1, P2, P3, P4>& f,
		 const bool retryIfInterrupted,
		 const Traced& location) throw():
	    _f(f),
	    _retryIfInterrupted(retryIfInterrupted),
	    _location(location)
	{
	}
	R operator()(P1 p1, P2 p2, P3 p3, P4 p4) const throw(
	    //
	    // pre: !retryIfInterrupted &&
	    //      signal interrupted the system call
	    //
	    SyscallInterrupted,
	    SyscallFailed);
    private:
	const SyscallF4<R, P1, P2, P3, P4> _f;
	const bool _retryIfInterrupted;
	const Traced _location;
    };
    
    
    template<class R, class P1, class P2, class P3, class P4, class P5>
    class Syscall5
    {
    public:
	Syscall5(const SyscallF5<R, P1, P2, P3, P4, P5>& f,
		 const bool retryIfInterrupted,
		 const Traced& location) throw():
	    _f(f),
	    _retryIfInterrupted(retryIfInterrupted),
	    _location(location)
	{
	}
	R operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const throw(
	    //
	    // pre: !retryIfInterrupted &&
	    //      signal interrupted the system call
	    //
	    SyscallInterrupted,
	    SyscallFailed);
    private:
	const SyscallF5<R, P1, P2, P3, P4, P5> _f;
	const bool _retryIfInterrupted;
	const Traced _location;
    };
    
    
    template<class R, class P1>
    R Syscall1<R, P1>::operator()(P1 p1) const throw(
	//
	// pre: !retryIfInterrupted &&
	//      signal interrupted the system call
	//
	SyscallInterrupted,
	SyscallFailed)
    {
	R status;
	while(((status=(*_f._f)(p1)) == _errorIndicator) &&
	      (errno == EINTR) &&
	      _retryIfInterrupted);
	if (status == _errorIndicator)
	{
	    if (errno == EINTR)
	    {
		throw SyscallInterrupted();
	    }
	    throw SyscallFailed(_f._name, errno, _location);
	}
	return status;
    }
    
    
    template<class R, class P1, class P2>
    R Syscall2<R, P1, P2>::operator()(P1 p1, P2 p2) const throw(
	//
	// pre: !retryIfInterrupted &&
	//      signal interrupted the system call
	//
	SyscallInterrupted,
	SyscallFailed)
    {
	R status;
	while(((status=(*_f._f)(p1, p2)) == _errorIndicator) &&
	      (errno == EINTR) &&
	      _retryIfInterrupted);
	if (status == _errorIndicator)
	{
	    if (errno == EINTR)
	    {
		throw SyscallInterrupted();
	    }
	    throw SyscallFailed(_f._name, errno, _location);
	}
	return status;
    }
    
    
    template<class R, class P1, class P2, class P3>
    R Syscall3<R, P1, P2, P3>::operator()(P1 p1, P2 p2, P3 p3) const throw(
	//
	// pre: !retryIfInterrupted &&
	//      signal interrupted the system call
	//
	SyscallInterrupted,
	SyscallFailed)
    {
	R status;
	while(((status=(*_f._f)(p1, p2, p3)) == -1) &&
	      (errno == EINTR) &&
	      _retryIfInterrupted);
	if (status == -1)
	{
	    if (errno == EINTR)
	    {
		throw SyscallInterrupted();
	    }
	    throw SyscallFailed(_f._name, errno, _location);
	}
	return status;
    }
    
    
    template<class R, class P1, class P2, class P3, class P4>
    R Syscall4<R, P1, P2, P3, P4>::operator()(
	P1 p1, P2 p2, P3 p3, P4 p4) const throw(
	//
	// pre: !retryIfInterrupted &&
	//      signal interrupted the system call
	//
	SyscallInterrupted,
	SyscallFailed)
    {
	R status;
	while(((status=(*_f._f)(p1, p2, p3, p4)) == -1) &&
	      (errno == EINTR) &&
	      _retryIfInterrupted);
	if (status == -1)
	{
	    if (errno == EINTR)
	    {
		throw SyscallInterrupted();
	    }
	    throw SyscallFailed(_f._name, errno, _location);
	}
	return status;
    }
    
    
    template<class R, class P1, class P2, class P3, class P4, class P5>
    R Syscall5<R, P1, P2, P3, P4, P5>::operator()(
	P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const throw(
	//
	// pre: !retryIfInterrupted &&
	//      signal interrupted the system call
	//
	SyscallInterrupted,
	SyscallFailed)
    {
	R status;
	while(((status=(*_f._f)(p1, p2, p3, p4, p5)) == -1) &&
	      (errno == EINTR) &&
	      _retryIfInterrupted);
	if (status == -1)
	{
	    if (errno == EINTR)
	    {
		throw SyscallInterrupted();
	    }
	    throw SyscallFailed(_f._name, errno, _location);
	}
	return status;
    }
    
    
    template<class R, class P1>
    Syscall1<R, P1>
    syscall(const SyscallF1<R, P1>& f,
	    const Traced& location, 
	    bool retryIfInterrupted,
	    const R errorIndicator) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed)
    {
	return Syscall1<R, P1>(f,
			       retryIfInterrupted,
			       location,
			       errorIndicator);
    }
    
    template<class R, class P1>
    Syscall1<R, P1>
    syscall(const char* name,
	    R (*const f)(P1),
	    const Traced& location, 
	    bool retryIfInterrupted,
	    const R errorIndicator) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed)
    {
	return Syscall1<R, P1>(
	    SyscallF1<R, P1>(name, f),
	    retryIfInterrupted,
	    location,
	    errorIndicator);
    }
    
    template<class R, class P1, class P2>
    Syscall2<R, P1, P2>
    syscall(const SyscallF2<R, P1, P2>& f,
	    const Traced& location,
	    bool retryIfInterrupted,
	    const R errorIndicator) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed)
    {
	return Syscall2<R, P1, P2>(f,
				   retryIfInterrupted,
				   location,
				   errorIndicator);
    }
    
    template<class R, class P1, class P2>
    Syscall2<R, P1, P2>
    syscall(const char* name,
	    R (*const f)(P1, P2),
	    const Traced& location,
	    bool retryIfInterrupted,
	    const R errorIndicator) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed)
    {
	return Syscall2<R, P1, P2>(
	    SyscallF2<R, P1, P2>(name, f),
	    retryIfInterrupted,
	    location,
	    errorIndicator);
    }
    
    template<class R, class P1, class P2, class P3>
    Syscall3<R, P1, P2, P3>
    syscall(const SyscallF3<R, P1, P2, P3>& f,
	    const Traced& location,
	    bool retryIfInterrupted) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed)
    {
	return Syscall3<R, P1, P2, P3>(f, retryIfInterrupted, location);
    }
    
    template<class R, class P1, class P2, class P3>
    Syscall3<R, P1, P2, P3>
    syscall(const char* name,
	    R (*const f)(P1, P2, P3),
	    const Traced& location,
	    bool retryIfInterrupted) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed)
    {
	return Syscall3<R, P1, P2, P3>(
	    SyscallF3<R, P1, P2, P3>(name, f),
	    retryIfInterrupted,
	    location);
    }

    template<class R, class P1, class P2, class P3, class P4>
    Syscall4<R, P1, P2, P3, P4>
    syscall(const SyscallF4<R, P1, P2, P3, P4>& f,
	    const Traced& location,
	    bool retryIfInterrupted) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed)
    {
	return Syscall4<R, P1, P2, P3, P4>(
	    f,
	    retryIfInterrupted,
	    location);
    }
    
    template<class R, class P1, class P2, class P3, class P4>
    Syscall4<R, P1, P2, P3, P4>
    syscall(const char* name,
	    R (*const f)(P1, P2, P3, P4),
	    const Traced& location,
	    bool retryIfInterrupted) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed)
    {
	return Syscall4<R, P1, P2, P3, P4>(
	    SyscallF4<R, P1, P2, P3, P4>(name, f),
	    retryIfInterrupted,
	    location);
    }

    template<class R, class P1, class P2, class P3, class P4, class P5>
    Syscall5<R, P1, P2, P3, P4, P5>
    syscall(const SyscallF5<R, P1, P2, P3, P4, P5>& f,
	    const Traced& location,
	    bool retryIfInterrupted) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed)
    {
	return Syscall5<R, P1, P2, P3, P4, P5>(
	    f,
	    retryIfInterrupted,
	    location);
    }
    
    template<class R, class P1, class P2, class P3, class P4, class P5>
    Syscall5<R, P1, P2, P3, P4, P5>
    syscall(const char* name,
	    R (*const f)(P1, P2, P3, P4, P5),
	    const Traced& location,
	    bool retryIfInterrupted) throw(
		//
		// pre: !retryIfInterrupted &&
		//      signal interrupted the system call
		//
		SyscallInterrupted,
		SyscallFailed)
    {
	return Syscall5<R, P1, P2, P3, P4, P5>(
	    SyscallF5<R, P1, P2, P3, P4, P5>(name, f),
	    retryIfInterrupted,
	    location);
    }
}


#endif
