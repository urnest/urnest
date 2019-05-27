// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
//    Example:
//
//    Lock mutex guarding data, and update data
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
#ifndef XJU_LOCK_HH
#define XJU_LOCK_HH

#include <xju/Mutex.hh>

#include <xju/assert.hh>

namespace xju
{

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
  // post: !mutex.isHeld()
  //
  ~Lock() throw();
  
  //
  // Check wether we hold specified mutex
  //
  bool holds(const Mutex& mutex) const throw();
  
private:
  Mutex& mutex_;
  std::unique_lock<std::mutex> l_;
  
  // not implemented
  Lock(const Lock&);
  Lock& operator=(const Lock&);

  friend class Condition;
};

inline Lock::Lock(Mutex& mutex) throw():
    mutex_(mutex),
    l_(mutex_._impl)
{
  mutex_._holder = this;
}

inline Lock::~Lock() throw()
{
  mutex_._holder = 0;
}

inline bool Lock::holds(const Mutex& mutex) const throw()
{
  return &mutex == &mutex_;
}

}

#endif
