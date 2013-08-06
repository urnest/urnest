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
//    Classes useful in multi-threaded programs.
//
//    Examples:
//
//    1. Lock mutex guarding data, and update data
//
//        Data data = ...;
//        Mutex guard;             assert(!guard.isHeld());
//
//        Lock l(guard);           assert(guard.isHeld());
//                                 assert(l.holds(guard));
//
//        data = new_value;
//
//
//    2. Update only if mutex is not held by someone else
//
//        Data data = ...;
//        Mutex guard;
//
//        TryLock t(guard);
//        if (t)
//        {
//                                 assert(t.holds(guard));
//           data = new_value;
//        }
//
//        
//    3. Signal condition variable.
//
//        Mutex guard;
//        Condition condition(guard);
//
//        Lock l(guard);           assert(l.holds(guard))
//        bool signalled = true;
//        condition.signal(l);
//                                 assert(l.holds(guard))
//
//    4. Wait for condition variable
//
//        Mutex guard;
//        Condition condition(guard);
//
//        Lock l(guard);           assert(l.holds(guard))
//        signalled = false;
//        while(!signalled)        // see [1]
//        {
//           condition.wait(l);
//        }
//                                 assert(l.holds(guard))
//
//
//    5. Conditionally wait for condition
//
//       Note that because Lock l takes over
//       from t, guard is unlocked as soon
//       as l is destroyed.
//
//        Mutex guard;
//        Condition condition(guard);
//
//        TryLock t(guard);
//        if (t)
//        {
//           Lock l(t);            assert(l.holds(guard))
//                                 assert(!t.holds(guard))
//           signalled = false;
//           while(!signalled)     // see [1]
//           {
//              condition.wait(l);
//           }
//           condition.wait(l);
//        }
//                                 assert(!t)
//                                 assert(!t.holds(guard))
//         
//
//        [1] Note that it is allowable, apparently, for
//            pthread_cond_[timed]wait to return 1 (before
//            the timeout) even though nobody has called
//            signal: so we have to have state explicitly
//            shared between the waiter/signaller.
//
//    6.Create a thread that calls x.f()
//
//        X x = ...;
//
//        Thread<X> t(x, &X::f);
//
//
//    7. Get two different identifiers.
//
//        Seq seq;
//        Seq::Value id1(seq);
//        Seq::Value id2(seq);
//
//   DESIGN NOTES:
//
//      This is not intended to be a full wrapper for all pthreads
//      functionality: it only provides enough functionality to support
//      use by other modules.
//
//      The design aims to make it as hard as possible to introduce
//      thread-safety bugs. For example, signalling a condition variable
//      checks that the caller holds the relevant lock (not holding the
//      relevant lock is a very commonly made and an expensive mistake).
//      The utility method used for such checks also make specifications
//      simple and clear.
//
//      The thread class deliberately does not use the traditional
//      invocation-via-base-class paradigm. In practice, allowing a
//      thread to invoke any object method is much easier to work with.
//      The ability to run a static function (i.e. a non-method) was not
//      provided as it does not allow any data to be passed to the thread,
//      and in practice such a thread is not useful without global shared
//      data (which is bad design).
//
//      The thread class deliberately has no "return value" mechanism.
//      Such a mechanism would unnecessarily complicate the Thread class,
//      and can be easily implemented, if required, by the application.
//
#ifndef _XJU_MT_HH_
#define _XJU_MT_HH_


#include <pthread.h>
#include <sys/errno.h>
#include <stdlib.h>
#include <xju/Time.hh>
#include <xju/assert.hh>
#include <string>


namespace xju
{
    namespace mt
    {
	class TryLock;
	class Lock;
	class Condition;
	
	class Mutex
	{
	public:
	    // post: not isHeld()
	    Mutex() throw();
	    
	    // pre: not isHeld()
	    ~Mutex() throw();
	    
	    //
	    // current holder, if any
	    //
	    // post: lifetime(result) = lifetime(this)
	    //
	    bool isHeld() const throw();
	    
	private:
	    pthread_mutex_t _impl;
	    void* _holder;				  // could be Lock or TryLock, for example
	    
	    friend class Lock;
	    friend class TryLock;
	    friend class Condition;
	    
	    // not implemented
	    Mutex(const Mutex&);
	    Mutex& operator=(const Mutex&);
	};
	
	
	class TryLock
	{
	public:
	    //
	    // post: (*this) or someone else was holding mutex.
	    //
	    explicit TryLock(Mutex& mutex) throw();
	    
	    //
	    // Did we succeed in locking mutex?
	    //
	    // post: result or not (*this).holds(mutex)
	    //
	    operator void*() const throw();
	    
	    //
	    // Do we hold the specified mutex?
	    //
	    bool holds(const Mutex& mutex) const throw();
	    
	    //
	    // post: !(*this)@pre or mutex not held by anyone
	    //
	    ~TryLock() throw();
	    
	private:
	    Mutex* _mutex;
	    
	    friend class Lock;
	    
	    // not implemented
	    TryLock(const TryLock&);
	    TryLock& operator=(const TryLock&);
	};
	
	
	class Lock
	{
	public:
	    //
	    // Wait for and acquire mutex.
	    //
	    // post: (*this).holds(mutex)
	    //
	    explicit Lock(Mutex& mutex) throw();
	    
	    //
	    // Take over holding l's mutex.
	    //
	    // (Does not block.)
	    //
	    // pre: l
	    // post: !l
	    //
	    Lock(TryLock& l) throw();
	    
	    //
	    // post: !mutex.isHeld()
	    //
	    ~Lock() throw();
	    
	    //
	    // Check wether we hold specified mutex
	    //
	    bool holds(const Mutex& mutex) const throw();
	    
	private:
	    Mutex& _mutex;
	    
	    // not implemented
	    Lock(const Lock&);
	    Lock& operator=(const Lock&);
	};
	
	
	class Condition
	{
	public:
	    //
	    // pre: lifetime(guard) > lifetime(this)
	    //
	    explicit Condition(Mutex& guard) throw();
	    
	    //
	    // pre: l.holds(guard)
	    //
	    void signal(const Lock& l) throw();
	    
	    
	    //
	    // Unlock guard, wait for signal() to be
	    // called, then relock guard.
	    //
	    // Note that this can return even though
	    // signal has not been called since we locked
	    // guard - see note [1] in Examples at top of file.
	    //
	    // pre: l.holds(guard)
	    // post: l.holds(guard)
	    //
	    void wait(Lock& l) throw();
	    
	    //
	    // Unlock guard, wait at most the specified amount
	    // of time for signal() to be called.
	    //
	    // Timeout is absolute.
	    //
	    // Note that return before timeout does not necessarily
	    // mean signal() has been called - see note [1].
	    //
	    void wait(Lock& l, const Time& timeout) throw();
	    
	    
	    ~Condition() throw();
	    
	    
	private:
	    Mutex& _guard;
	    pthread_cond_t _impl;
	};
	
	
	template<class T>
	class Thread
	{
	public:
	    //
	    // creates and starts thread, which will
	    // call (t.*f)() once before returning 0.
	    //
	    // pre: lifetime(t) > lifetime(this)
	    //
	    Thread(T& t, void (T::*f)() throw());
	    
	    //
	    // waits for and joins with thread (i.e.
	    // waits for (t.*f)() to return).
	    //
	    // (Note that client must have somehow
	    // arranged for thread to end eventually (i.e.
	    // for f to return.)
	    //
	    ~Thread() throw();
	    
	private:
	    T& _t;
	    void (T::*_f)() throw();
	    
	    pthread_t _impl;
	    
	    static void* main(void* x)
	    {
		Thread<T>* xx = (Thread<T>*)x;
		((&xx->_t)->*(xx->_f))();
		return 0;
	    }
	};
	
	class Seq
	{
	public:
	    //
	    // Construct a sequence that will go through the
	    // same values as other sequences constructed in
	    // the same way.
	    //
	    // post: operator() = Seq().operator()
	    //
	    Seq() throw();
	    
	    //
	    // Construct a sequence that uses the specified
	    // initial value to construct the first value.
	    //
	    // (This allows a client to construct a sequence
	    // after another has been destroyed in such a way
	    // that the two sequences have no values in common.)
	    //
	    Seq(const Time& startAfter) throw();
	    
	    
	    class Value
	    {
	    public:
				//
				// Take a value unique amongst values constructed
				// from seq, and greater than all values previously
				// constructed from seq.
				//
		Value(Seq& seq) throw();
		
		bool operator==(const Value& b) const throw();
		bool operator<(const Value& b) const throw();
		
				//
				// A string representation of (*this). Not necessarily
				// human-readable.
				//
		std::string stringRepr() const throw();
		
				//
				// Recover Value from stringRepr() representation.
				//
				// pre: stringRepr was returned from stringRepr()
				//
		explicit Value(const std::string& stringRepr) throw();
		
	    private:
		Time _value;
	    };
	    friend class Value;
	    
	private:
	    Mutex _guard;
	    Time _last;
	    
	    Time next() throw();
	};
	
	
	inline Mutex::Mutex() throw():
	    _holder(0)
	{
	    if (pthread_mutex_init(&_impl, 0) == -1)
	    {
		int error = errno;
		abort();
	    }
	}
	
	inline Mutex::~Mutex() throw()
	{
	    assert_equal(_holder, (void*)0);
	    
	    pthread_mutex_destroy(&_impl);
	}
	
	inline bool Mutex::isHeld() const throw()
	{
	    return _holder != 0;
	}
	
	inline TryLock::TryLock(Mutex& mutex) throw():
	    _mutex(0)
	{
	    int i = pthread_mutex_trylock(&(mutex._impl));
	    if (i == 1)
	    {
		_mutex = &mutex;
		mutex._holder = this;
	    }
	    else if (i == -1)
	    {
		int error = errno;
		abort();
	    }
	}
	
	inline TryLock::operator void*() const throw()
	{
	    return _mutex;
	}
	
	inline TryLock::~TryLock() throw()
	{
	    if (_mutex)
	    {
		pthread_mutex_unlock(&(_mutex->_impl));
	    }
	}
	
	inline Lock::Lock(Mutex& mutex) throw():
	    _mutex(mutex)
	{
	    if (pthread_mutex_lock(&mutex._impl) == -1)
	    {
		int error = errno;
		abort();
	    }
	    _mutex._holder = this;
	}
	
	inline Lock::Lock(TryLock& l) throw():
	    _mutex(*l._mutex)
	{
	    _mutex._holder = this;
	    l._mutex = 0;
	}
	
	inline Lock::~Lock() throw()
	{
	    _mutex._holder = 0;
	    pthread_mutex_unlock(&_mutex._impl);
	}
	
	inline bool Lock::holds(const Mutex& mutex) const throw()
	{
	    return &mutex == &_mutex;
	}
	
	
	
	template<class T>
	Thread<T>::Thread(T& t, void (T::*f)() throw()):
	    _t(t),
	    _f(f)
	{
	    if (pthread_create(&_impl, 0, main, this) == -1)
	    {
		int error = errno;
		abort();
	    }
	}
	
	template<class T>
	Thread<T>::~Thread() throw()
	{
	    void* status;
	    
	    pthread_join(_impl, &status);
	    pthread_detach(_impl);
	}
	
	
	inline Seq::Seq() throw():
	    _last(0, 0)
	{
	}
	
	inline Seq::Seq(const Time& startAfter) throw():
	    _last(startAfter)
	{
	}
	
	inline Seq::Value::Value(Seq& seq) throw():
	    _value(seq.next())
	{
	}
	
	inline bool Seq::Value::operator==(const Seq::Value& b) const throw()
	{
	    return _value == b._value;
	}
	
	inline bool Seq::Value::operator<(const Seq::Value& b) const throw()
	{
	    return _value < b._value;
	}
    }
}

#endif

