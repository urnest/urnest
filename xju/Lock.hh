// Copyright (c) 2014 Trevor Taylor
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all.
// Trevor Taylor makes no representations about the suitability of this
// software for any purpose.  It is provided "as is" without express or
// implied warranty.
//
#ifndef XJU_LOCK_HH
#define XJU_LOCK_HH

#include "Mutex.hh"

#include <pthread.h>
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
  Mutex& _mutex;
  
  // not implemented
  Lock(const Lock&);
  Lock& operator=(const Lock&);
};

inline Lock::Lock(Mutex& mutex) throw():
    _mutex(mutex)
{
  assert_equal(pthread_mutex_lock(&mutex._impl),0);
  _mutex._holder = this;
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

}

#endif
